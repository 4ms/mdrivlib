#pragma once
#include "drivers/i2c.hh"
#include "fusb302_registers.hh"
#include <cstdint>

// Debugging:
#define FUSBDEBUG

#ifdef FUSBDEBUG
#include <cstdio>
#define pr_debug printf
#include <optional>
#include <string_view>
#else
static inline void pr_debug(...) {
}
#endif

namespace FUSB302
{

struct Device {

	mdrivlib::I2CPeriph &i2c;
	uint8_t dev_addr;
	uint8_t device_id = 0;

	enum class ConnectedState { None, TogglePolling, AsHost, AsDevice } state = ConnectedState::None;

	Device(mdrivlib::I2CPeriph &i2c, uint8_t device_addr)
		: i2c{i2c}
		, dev_addr{device_addr} {
	}

	bool init() {
		uint8_t data;
		data = read<ID>();
		if (!data)
			return false;

		device_id = data;

		// Read Interrupt registers to clear them
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();
		return true;
	}

	uint8_t get_chip_id() {
		return device_id;
	}

	// Software-reset the FUSB302: all registers return to their power-on defaults
	// and any pending interrupts are cleared.
	void reset() {
		write<Reset>({.SWReset = 1});
		HAL_Delay(10); // let the POR settle before further register access
		// Interrupt registers are cleared on read
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();
	}

	// Auto host/device: DRP toggle, settling to either role depending on what's
	// attached
	void start_drp_polling() {
		start_toggle_polling(Control2::PollDRP);
	}

	// Force host role: SRC-only toggle. The port presents Rp (pull-up) and only
	// ever attaches as a host (to a device/sink). Will not attach to another host.
	void start_src_polling() {
		start_toggle_polling(Control2::PollSRC);
	}

	// Force device role: SNK-only toggle. The port presents Rd (pull-down) and
	// only ever attaches as a device (to a host/source).
	void start_snk_polling() {
		start_toggle_polling(Control2::PollSNK);
	}

	// Start the FUSB302 toggle state machine in the given polling mode (Control2
	// PollDRP/PollSRC/PollSNK). All three roles use the same attach/detach
	// detection: on settle, I_TOGGLE fires and Status1A.TOGSS reports the outcome
	// (decoded in handle_interrupt). Only the set of roles toggled differs.
	void start_toggle_polling(uint8_t polling_mode) {
		reset();

		// Setup per datasheet p. 7 (Toggle Functionality)
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 0});

		// Clear interrupts
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();

		// Turn toggle bit off, then on (otherwise TOGGLE mode is not re-started)
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		HAL_Delay(10);
		write<Control2>({.Toggle = 1, .PollingMode = polling_mode, .ToggleIgnoreRa = 1});

		write<Switches0>({.ConnectVConnCC1 = 0, .ConnectVConnCC2 = 0});
		// Note: setting Mask::VBusOK to 0 when HostCurrentReq is 0 results in it
		// not detecting disconnect as host
		write<Mask>({.HostCurrentReq = 0,
					 .Collision = 1,
					 .Wake = 1,
					 .Alert = 1,
					 .CRCCheck = 1,
					 .CompChange = 1,
					 .CCBusActivity = 1,
					 .VBusOK = 1});
		write<MaskA>({.HardResetRx = 1,
					  .SoftResetRx = 1,
					  .TxSent = 1,
					  .HardResetSent = 1,
					  .RetryFail = 1,
					  .SoftFail = 1,
					  .ToggleDone = 0,
					  .OCPTempEvent = 1});
		write<MaskB>({.GoodCRCSent = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureBlock = 0, .RXAndCurrentRefs = 0, .IntOsc = 0});

		dump_all_regs();

		state = ConnectedState::TogglePolling;
	}

	ConnectedState get_state() {
		return state;
	}

	void handle_interrupt() {
		auto intr = read<Interrupt>();
		auto intrA = read<InterruptA>();
		auto intrB = read<InterruptB>();

		pr_debug("Int = 0x%x VBusOK=%d, BCLVL=%d\n", (uint8_t)intr, intr.VBusOK, intr.BCLevel);
		pr_debug("IntA = 0x%x IntB = 0x%x TogDone=%d\n", (int)intrA, (int)intrB, intrA.ToggleDone);

		switch (state) {
			case ConnectedState::TogglePolling: {
				pr_debug("State is currently Polling\n");

				Status0 status0{read<Status0>()};
				Status1A status1a{read<Status1A>()};

				// VBUS present while we are only polling means the *partner* is
				// sourcing it -- we never drive our own 5V source during polling --
				// so the partner is the host and we must attach as a device,
				// whatever the DRP toggle reported. The FUSB302 DRP toggle can
				// otherwise settle to SRC against a powered host (a sampling race),
				// causing a host-host conflict and VBUS contention.
				if (status0.VBusOK) {
					state = ConnectedState::AsDevice;

					// If the toggle did not itself settle as SNK (it settled SRC, or
					// hadn't settled yet), the chip is still presenting Rp: the
					// partner never sees our Rd (a compliant host won't even source
					// VBUS to us), and the Rp-vs-Rp contention on CC causes an
					// endless BC_LVL/COMP_CHNG interrupt storm. Manually drop the
					// chip into the attached-sink configuration: stop the toggle,
					// present Rd on both CC pins, and measure the live CC line.
					if (!status1a.ToggleOutcomeIsSink) {
						write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
						write<Power>(
							{.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 0, .IntOsc = 0});

						// Find the live CC by probing: measure each CC in turn until
						// the host's Rp is seen (BCLevel > 0). The CC that TOGSS
						// flagged is only a hint -- on a mis-settle it can point at
						// the open CC line, and measuring the open line means
						// BC_LVL never changes again, so unplug goes undetected.
						uint8_t cc2 = status1a.ToggleOutcomeIsCC2;
						for (auto tries = 0; tries < 2; tries++) {
							write<Switches0>({.PullDownCC1 = 1,
											  .PullDownCC2 = 1,
											  .MeasureCC1 = uint8_t(cc2 ? 0 : 1),
											  .MeasureCC2 = cc2});
							HAL_Delay(2); // let the BC_LVL comparator settle
							Status0 probe{read<Status0>()};
							pr_debug("Sink override: CC%d BCLevel=%d\n", cc2 ? 2 : 1, probe.BCLevel);
							if (probe.BCLevel > 0)
								break;
							cc2 = cc2 ? uint8_t{0} : uint8_t{1};
						}
					}
				}

				else if (status1a.ToggleOutcomeIsSink)
					state = ConnectedState::AsDevice;

				else if (status1a.ToggleOutcomeIsCC1 || status1a.ToggleOutcomeIsCC2)
					state = ConnectedState::AsHost;

				// As a device (sink), the clean detach signal is VBUS loss, so
				// unmask the VBusOK interrupt for reliable disconnect detection.
				// Otherwise only BC_LVL is unmasked, and an OXI-style host+power
				// unplug (VBUS and CC drop together) intermittently produces no
				// interrupt, leaving us stuck in AsDevice. VBusOK stays masked
				// while polling/host (set in start_toggle_polling, which re-arms
				// the mask on the next re-poll) because as a source we drive VBUS
				// ourselves and it would race host-unplug detection.
				if (state == ConnectedState::AsDevice)
					write<Mask>({.HostCurrentReq = 0,
								 .Collision = 1,
								 .Wake = 1,
								 .Alert = 1,
								 .CRCCheck = 1,
								 .CompChange = 1,
								 .CCBusActivity = 1,
								 .VBusOK = 0});

				// could also check Status0: Comp == 0 && BCLevel < 3
				// Comp == 0 means CC pin is read as less than reference, meaning device
				// Rd pull-down was detected BC<3 means CC pin is read as < 1.23V, meaning
				// a device Rd pull-down was detected
			} break;

			case ConnectedState::AsDevice: {
				pr_debug("State is currently Device\n");
				Status0 status0{read<Status0>()};

				// Look for unplug event:
				// VBusOK = 0 means no VBUS, BCLevel == 0 means CC detected as low (no
				// host pull-up detected)
				if (status0.VBusOK == 0 || status0.BCLevel == 0)
					state = ConnectedState::None;
			} break;

			case ConnectedState::AsHost: {
				pr_debug("State is currently Host\n");
				Status0 status{read<Status0>()};
				// Look for Unplug event:
				// Comp==1, BC==3 means CC pin is read as > 1.23V, meaning no device Rd
				// pull-down
				// FIXME: why isn't Comp set at this point? it gets set a moment later...
				if (/*status.Comp == 1 &&*/ status.BCLevel == 3)
					state = ConnectedState::None;
			} break;

			case ConnectedState::None: {
				pr_debug("State is currently None\n");
			} break;
		}
	}

	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c.write_reg(dev_addr, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	template<typename Reg>
	Reg read() {
		static bool got_error_once = false;
		auto reg = i2c.read_reg<Reg>(dev_addr);

		if (!reg.has_value() && !got_error_once) {
			pr_debug("Error reading Reg 0x%x\n", Reg::Address);
			got_error_once = true;
		}
		return reg.value_or(Reg::make(0));
	}

	//// Debugging stuff:
#ifdef FUSBDEBUG
	template<typename Reg>
	requires std::derived_from<Reg, BusReg::ReadAccess>
	void reg_check_changed(std::string_view regname) {
		static uint8_t last_val = 0xFF;
		auto val = read<Reg>();
		if ((uint8_t)val != last_val)
			pr_debug("Changed %s: 0x%x\n", regname.data(), (int)val);
		last_val = val;
	}

	template<typename Reg>
	requires std::derived_from<Reg, BusReg::ReadAccess>
	void reg_dump(std::string_view regname) {
		auto val = read<Reg>();
		pr_debug("%s: 0x%x\n", regname.data(), (uint8_t)val);

		// Dump individual fields of interest:
		if constexpr (static_cast<uint8_t>(Reg::Address) == FUSB302::Status0::Address) {
			auto s = Status0::make(val);
			pr_debug("   BCLevel=%d Wake=%d Comp=%d VBusOK=%d\n", s.BCLevel, s.Wake, s.Comp, s.VBusOK);
		}
	}

	void check_all_regs() {
		reg_check_changed<ID>("ID");
		reg_check_changed<Switches0>("Switches0");
		reg_check_changed<Switches1>("Switches1");
		reg_check_changed<Measure>("Measure");
		reg_check_changed<Slice>("Slice");
		reg_check_changed<Control0>("Control0");
		reg_check_changed<Control1>("Control1");
		reg_check_changed<Control2>("Control2");
		reg_check_changed<Control3>("Control3");
		reg_check_changed<Mask>("Mask1");
		reg_check_changed<Power>("Power");
		reg_check_changed<OCP>("OCP");
		reg_check_changed<MaskA>("MaskA");
		reg_check_changed<MaskB>("MaskB");
		reg_check_changed<Control4>("Control4");
		reg_check_changed<Status0A>("Status0A");
		reg_check_changed<Status1A>("Status1A");
		reg_check_changed<Status0>("Status0");
		reg_check_changed<Status1>("Status1");
	}

	void dump_all_regs() {
		reg_dump<ID>("ID");
		reg_dump<Switches0>("Switches0");
		reg_dump<Switches1>("Switches1");
		reg_dump<Measure>("Measure");
		reg_dump<Slice>("Slice");
		reg_dump<Control0>("Control0");
		reg_dump<Control1>("Control1");
		reg_dump<Control2>("Control2");
		reg_dump<Control3>("Control3");
		reg_dump<Mask>("Mask1");
		reg_dump<Power>("Power");
		reg_dump<OCP>("OCP");
		reg_dump<MaskA>("MaskA");
		reg_dump<MaskB>("MaskB");
		reg_dump<Control4>("Control4");
		reg_dump<Status0A>("Status0A");
		reg_dump<Status1A>("Status1A");
		reg_dump<Status0>("Status0");
		reg_dump<Status1>("Status1");
	}
#else
	void dump_all_regs() {
	}
	void check_all_regs() {
	}
#endif
};

} // namespace FUSB302
