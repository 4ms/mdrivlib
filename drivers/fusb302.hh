#pragma once
#include "drivers/i2c.hh"
#include "fusb302_registers.hh"
#include <cstdint>

// Debugging:
// #define FUSBDEBUG

#ifdef FUSBDEBUG
#include <cstdio>
#define pr_debug printf
#include <optional>
#include <string_view>
#else
static inline void pr_debug(...) {
}
#endif

// Note on connecting to self-powered devices that constantly toggle CC lines
// e.g. OXI One mkI in Self Powered Device mode (note that the OXI is compliant
// in Device mode, it's just "Device Self Powered" mode that is non-compliant)
//
// When we plug into a device like this, we see VBUS high from the partner,
// so we present a Rd (advertise ourselves as device/sink).
// If the other machine doesn't enumerate us, then we keep trying another way.
//
// OXI One is particularly strange in "Device Self Powered" mode in that it continues
// to toggle Rp/no-Rp even after it seemingly has decided on a role, which means we
// can't just sample the value once and know what role it intends to take. So we
// have to detect the toggling itself by sampling over a period, and then we know it's
// a non-compliant device and we can try to connect.
//
// When our side presents an Rd, the FUSB measures BCLevel=1/2/3 when the OXI has Rp
// (OXI advertises 1.5A so we read 2), and 0 when it doesn't.
// Steady-Rp partners (real hosts) are identified by the first quick probe, so seeing no Rp
// at first and then Rp within a ~120ms window, all while VBUS is high, means the partner is
// toggling: not a normal host or device. So we flag it as a Self Powered Device.
//
// To connect, we present an Rp to advertise as a host, and stop responding to CC toggles.
// We do not supply VBUS since it's already present.
//
// Another way we detect the OXI is when we see VBUS high and an Rp and then start to attach as a
// device. Then before we've been enumerated we see BCLevel drop to 0 while VBUS is still high,
// meaning the partner dropped its Rp and is now advertising as a device, yet it keeps sourcing
// VBUS. If the partner had been unplugged, we'd see VBUS drop, so this can only be a Self Powered Device.
//
// Basically, the OXI's VBUS source when in Self Powered Device mode is unconditional, decoupled from its CC role.
// From then on we ignore its CC entirely and watch VBUS to detect detach events.

// Needs pr_debug, so included here rather than at the top
#include "drivers/fusb302_pd.hh"

namespace FUSB302
{

struct Device {

	mdrivlib::I2CPeriph &i2c;
	uint8_t dev_addr;
	uint8_t device_id = 0;

	enum class ConnectedState { None, TogglePolling, AsHost, AsDevice } state = ConnectedState::None;
	uint8_t last_polling_mode = Control2::PollDRP;

	// AsDevice unplug debounce (see handle_interrupt AsDevice case):
	// 0 = link reads up; else tick when the current down-episode began
	uint32_t link_down_since = 0;
	uint32_t link_down_last_seen = 0;
	static constexpr uint32_t LinkDownDebounceMs = 350;
	static constexpr uint32_t LinkDownEpisodeMs = 600; // > UsbManager's 250ms link poll period

	// Off until the data connection is established (set by the USB manager once
	// enumeration succeeds). Before that, a VBUS/CC dip is meaningful (see the
	// AsDevice handler); after enumeration, dips are transient noise that must
	// not kill a working session.
	bool link_down_debounce_enabled = false;

	// Set when attached AsHost to a partner that sources VBUS itself (toggling
	// self-powered DRP like the OXI One, or a backfeeding gadget rig). For
	// these partners VBUS-loss is the only trustworthy detach signal: the OXI
	// keeps toggling its CC even while enumerated, so BC_LVL flaps between
	// "Rd present" and "open" forever and must be ignored.
	bool host_partner_sources_vbus = false;

	void set_link_debounce(bool enabled) {
		link_down_debounce_enabled = enabled;
		if (!enabled)
			link_down_since = 0;
	}

	// Minimal PD sink engine (contract + DR_Swap); see fusb302_pd.hh. Enabled
	// automatically on sink attach; policy calls (request_dr_swap) come from
	// the USB manager.
	PDSink<Device> pd{*this};

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

	// One-shot "is a device attached downstream?" check, for use while forced
	// into the device (SNK) role. In SNK polling the port presents Rd and is
	// electrically blind to a downstream device: that device also presents Rd
	// and sources no VBUS, so two Rd's face each other and nothing is detected
	// (a USB drive plugged in here does nothing). The only way to sense it is to
	// momentarily present Rp -- the host signature -- and look for the partner's
	// Rd pulling a CC line into range, exactly as a host/SRC toggle would.
	//
	// This stops the toggle, presents Rp on both CC pins, measures each, then
	// re-arms SNK toggle polling and returns true if a device was seen. It never
	// sources VBUS or VCONN, so nothing downstream is powered or enumerated -- it
	// only senses presence. Call it periodically while idle (state stays
	// TogglePolling); the brief Rp window means a host attaching at that instant
	// is simply caught on the next SNK poll instead. Expects to be called while
	// SNK polling (it restores SNK polling on exit).
	bool probe_snk_for_device() {
		// Stop the toggle state machine so it can neither fire I_TOGGLE nor drive
		// the CC pins while we measure them manually.
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});

		// Power up for a real CC measurement. NB the Power fields are misnamed vs
		// the FUSB302 datasheet: "MeasureBlock" is PWR1 (receiver + current
		// references) and "RXAndCurrentRefs" is PWR2 (the actual measure block).
		// Both are required for a valid BC_LVL reading.
		write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 0});
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 0});

		bool device_present = false;
		for (uint8_t cc2 = 0; cc2 <= 1 && !device_present; cc2++) {
			write<Switches0>({.MeasureCC1 = uint8_t(cc2 ? 0 : 1), .MeasureCC2 = cc2, .PullUpCC1 = 1, .PullUpCC2 = 1});
			HAL_Delay(2); // let the BC_LVL comparator settle
			Status0 probe{read<Status0>()};
			pr_debug("Device probe: CC%d BCLevel=%d\n", cc2 ? 2 : 1, probe.BCLevel);
			// With Rp presented, BC_LVL reads the partner's pull-down: 3 (0b11) is
			// an open line (no device), 0 is Ra/VCONN only (a powered cable, not a
			// device); 1 or 2 means a device's Rd is pulling CC into range.
			if (probe.BCLevel == 1 || probe.BCLevel == 2)
				device_present = true;
		}

		// Restore SNK toggle polling: drop the manual Rp, power the measure block
		// back down (the toggle SM manages it), clear any interrupt the manual CC
		// changes latched, and re-enable the SNK toggle. The toggle bit has been
		// off throughout the measurement (well over the off->on settle time), so
		// no extra delay is needed before re-enabling it.
		write<Switches0>({.ConnectVConnCC1 = 0, .ConnectVConnCC2 = 0});
		write<Power>({.BandGapAndWake = 1, .MeasureBlock = 0, .RXAndCurrentRefs = 0, .IntOsc = 0});
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();
		write<Control2>({.Toggle = 1, .PollingMode = Control2::PollSNK, .ToggleIgnoreRa = 1});

		return device_present;
	}

	// Start the FUSB302 toggle state machine in the given polling mode (Control2
	// PollDRP/PollSRC/PollSNK). All three roles use the same attach/detach
	// detection: on settle, I_TOGGLE fires and Status1A.TOGSS reports the outcome
	// (decoded in handle_interrupt). Only the set of roles toggled differs.
	void start_toggle_polling(uint8_t polling_mode) {
		last_polling_mode = polling_mode;
		set_link_debounce(false); // re-enabled by the manager once enumerated
		host_partner_sources_vbus = false;
		pd.on_chip_reset(); // SWReset below wipes the chip's PD block
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

	// Attach as host to a self-powered partner by presenting steady Rp on both
	// CCs (source persona), measuring the given CC. Scope-verified against the
	// OXI One "Device Self Powered": it latches its *device* persona when its
	// CC sees Rp (D+ rises ~250ms later) even though its own CC keeps
	// DRP-toggling forever -- so there is no Rd settle to wait for, and detach
	// detection must ride on VBUS (host_partner_sources_vbus).
	void attach_as_static_src(bool meas_cc2) {
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 0});
		write<Switches0>({.MeasureCC1 = uint8_t(meas_cc2 ? 0 : 1),
						  .MeasureCC2 = uint8_t(meas_cc2 ? 1 : 0),
						  .PullUpCC1 = 1,
						  .PullUpCC2 = 1});
		// The partner's CC toggling would storm BC_LVL/COMP interrupts: mask
		// them; unmask VBusOK (rise is reliable; loss is backstopped by the
		// manager's poll)
		write<Mask>({.HostCurrentReq = 1,
					 .Collision = 1,
					 .Wake = 1,
					 .Alert = 1,
					 .CRCCheck = 1,
					 .CompChange = 1,
					 .CCBusActivity = 1,
					 .VBusOK = 0});
		host_partner_sources_vbus = true;
		pd.on_chip_reset(); // partner classified non-PD; stop servicing
		state = ConnectedState::AsHost;
	}

	// --- Characterization/experiment support ---

	// Present Rd on both CC pins, statically: no toggle state machine, no
	// interrupts (caller polls), measure block powered. The partner sees a
	// plain attached sink and nothing we do ever perturbs the CC lines.
	void configure_static_sink() {
		reset();
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 1});
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 0});
		write<Switches0>({.PullDownCC1 = 1, .PullDownCC2 = 1, .MeasureCC1 = 1, .MeasureCC2 = 0});
		// Everything masked; the experiment loop polls Status0
		write<Mask>(Mask::make(0xFF));
		write<MaskA>(MaskA::make(0xFF));
		write<MaskB>(MaskB::make(0x01));
		state = ConnectedState::AsDevice;
	}

	// Present Rp on both CC pins, statically (source persona): no toggle, no
	// interrupts, measure block powered. BC_LVL semantics with Rp presented:
	// 3 = open, 0 = Ra, 1/2 = partner Rd.
	void configure_static_src() {
		reset();
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 1});
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 0});
		write<Switches0>({.MeasureCC1 = 1, .MeasureCC2 = 0, .PullUpCC1 = 1, .PullUpCC2 = 1});
		write<Mask>(Mask::make(0xFF));
		write<MaskA>(MaskA::make(0xFF));
		write<MaskB>(MaskB::make(0x01));
		state = ConnectedState::AsHost;
	}

	struct CCLevels {
		uint8_t cc1;
		uint8_t cc2;
		uint8_t vbusok;
	};

	// Measure both CC lines (BC_LVL) and VBUS. Only the internal measurement
	// mux is switched; what we present on the pins is untouched.
	CCLevels read_both_cc() {
		auto sw = read<Switches0>();
		sw.MeasureCC1 = 1;
		sw.MeasureCC2 = 0;
		write<Switches0>(sw);
		HAL_Delay(1);
		Status0 s1{read<Status0>()};
		sw.MeasureCC1 = 0;
		sw.MeasureCC2 = 1;
		write<Switches0>(sw);
		HAL_Delay(1);
		Status0 s2{read<Status0>()};
		return {s1.BCLevel, s2.BCLevel, s2.VBusOK};
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

				if (status1a.ToggleOutcomeIsSink) {
					// The toggle settled as an attached sink: the partner presents Rp,
					// so it is the host (whether or not it sources VBUS).
					// Take over from the toggle state machine: stop it, present Rd on
					// both CCs and measure the settled CC, so BC_LVL detach detection
					// and the PD engine's BMC TX/RX have a defined, live CC.
					uint8_t cc2 = status1a.ToggleOutcomeIsCC2;
					write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
					write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 0});
					write<Switches0>(
						{.PullDownCC1 = 1, .PullDownCC2 = 1, .MeasureCC1 = uint8_t(cc2 ? 0 : 1), .MeasureCC2 = cc2});
					state = ConnectedState::AsDevice;
					pd.enable(cc2);
				}

				// VBUS present while we are only polling means the *partner* drives
				// it -- we never drive our own 5V source during polling -- but that
				// alone does not prove the partner is a host. Two very different
				// partners look like this when the toggle did not settle as SNK:
				//  - A powered host the DRP toggle mis-settled against (a sampling
				//    race): it presents Rp. We must attach as a sink -- staying SRC
				//    means Rp-vs-Rp contention on CC and an endless BC_LVL/COMP_CHNG
				//    interrupt storm.
				//  - A self-powered *device* that backfeeds VBUS while presenting Rd
				//    (RPi USB gadget rigs, OXI One): here the toggle settled SRC
				//    *correctly*. Forcing sink would leave neither side presenting
				//    Rp, so BC_LVL reads 0 and we would bounce attach/detach forever
				//    without ever enumerating it.
				// Distinguish them by measuring CC ourselves: stop the toggle,
				// present Rd on both CC pins, and look for the partner's Rp.
				else if (status0.VBusOK)
				{
					write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});

					// Enable PWR1 (MeasureBlock) and PWR2 RXAndCurrentRefs):
					// we just stopped the toggle state machine (line above), so the chip is
					// no longer powering the measure block for us. With PWR2 off, BC_LVL reads 0
					// forever and the 250ms link-check backstop sees a false detach every cycle.
					write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 0});

					// Find the live CC by probing: measure each CC in turn until
					// the host's Rp is seen (BCLevel > 0). The CC that TOGSS
					// flagged is only a hint -- on a mis-settle it can point at
					// the open CC line, and measuring the open line means
					// BC_LVL never changes again, so unplug goes undetected.
					uint8_t cc2 = status1a.ToggleOutcomeIsCC2;
					bool host_rp_found = false;
					// In deliberate SRC-only polling (ForceHost, or the manager's
					// Try.SRC pass for a self-powered device), a TOGDONE means the
					// partner's Rd landed on our Rp: it took the sink role. Never
					// yield to sink here -- the sink-override protection below is
					// for DRP auto-attach only (a real host never fires TOGDONE in
					// SRC polling: Rp-vs-Rp detects nothing).
					bool src_only = last_polling_mode == Control2::PollSRC;
					for (auto tries = 0; tries < 2 && !src_only; tries++) {
						write<Switches0>({.PullDownCC1 = 1,
										  .PullDownCC2 = 1,
										  .MeasureCC1 = uint8_t(cc2 ? 0 : 1),
										  .MeasureCC2 = cc2});
						HAL_Delay(2); // let the BC_LVL comparator settle
						Status0 probe{read<Status0>()};
						pr_debug("Sink override: CC%d BCLevel=%d\n", cc2 ? 2 : 1, probe.BCLevel);
						if (probe.BCLevel > 0) {
							host_rp_found = true;
							break;
						}
						cc2 = cc2 ? uint8_t{0} : uint8_t{1};
					}

					// Two quick reads can catch a *toggling* partner (a DRP, or the
					// OXI One in Device Self Powered mode) in its Rd phase and
					// mistake it for a static self-powered device -- then we attach
					// as host and collide Rp-vs-Rp when the partner flips back.
					// Before concluding "no host here", keep our Rd presented and
					// watch both CCs across a full DRP toggle period (tDRP <= 100ms,
					// Rp phase >= 30% duty). Blocks ~120ms, only on this ambiguous
					// attach path.
					bool partner_toggling = false;
					if (!host_rp_found && !src_only) {
						for (auto rounds = 0; rounds < 6 && !host_rp_found; rounds++) {
							for (uint8_t probe_cc2 = 0; probe_cc2 <= 1 && !host_rp_found; probe_cc2++) {
								write<Switches0>({.PullDownCC1 = 1,
												  .PullDownCC2 = 1,
												  .MeasureCC1 = uint8_t(probe_cc2 ? 0 : 1),
												  .MeasureCC2 = probe_cc2});
								HAL_Delay(10);
								Status0 probe{read<Status0>()};
								if (probe.BCLevel > 0) {
									pr_debug("Sink override (extended): CC%d BCLevel=%d\n",
											 probe_cc2 ? 2 : 1,
											 probe.BCLevel);
									host_rp_found = true; // leave this CC selected for detach detection
									cc2 = probe_cc2;
									// The quick probe just saw this partner NOT
									// presenting Rp: it is alternating Rp/Rd (a DRP)
									partner_toggling = true;
								}
							}
						}
					}

					// A toggling partner that also backfeeds VBUS is a self-powered
					// DRP that wants the *device* data role (OXI One "Device Self
					// Powered"): it never enumerates anyone and never holds a CC
					// role, but it latches its device persona on seeing steady Rp
					// (scope-verified: D+ ~250ms later). Host it directly.
					if (partner_toggling) {
						pr_debug("Toggling self-powered partner: presenting Rp to host it\n");
						attach_as_static_src(cc2);
					}

					else if (host_rp_found)
					{
						// A steady host Rp is out there: attach as a sink. The live
						// CC is left selected for measurement, so detach detection
						// (VBUS loss or BC_LVL 0) keeps working.
						state = ConnectedState::AsDevice;
						pd.enable(cc2);
					} else {
						// No Rp on either CC: the VBUS is backfed by a self-powered
						// device, and the toggle's SRC outcome was right. Attach as
						// host: present Rp on both CC pins and select the CC where
						// the partner's Rd is for measurement, so the AsHost detach
						// check (BC_LVL == 3, line open) keeps working.
						cc2 = status1a.ToggleOutcomeIsCC2;
						bool device_rd_found = false;
						for (auto tries = 0; tries < 2; tries++) {
							write<Switches0>({.MeasureCC1 = uint8_t(cc2 ? 0 : 1),
											  .MeasureCC2 = cc2,
											  .PullUpCC1 = 1,
											  .PullUpCC2 = 1});
							HAL_Delay(2); // let the BC_LVL comparator settle
							Status0 probe{read<Status0>()};
							pr_debug("Source override: CC%d BCLevel=%d\n", cc2 ? 2 : 1, probe.BCLevel);
							// With Rp presented: 3 is an open line, 0 is Ra only
							// (powered cable); 1 or 2 means a device's Rd.
							if (probe.BCLevel == 1 || probe.BCLevel == 2) {
								device_rd_found = true;
								break;
							}
							cc2 = cc2 ? uint8_t{0} : uint8_t{1};
						}

						if (device_rd_found) {
							// One Rd sample is not an attach: a toggling DRP (OXI
							// One) alternates Rd/Rp until it has seen our steady
							// Rp for its debounce time (tCCDebounce 100-200ms).
							// Attaching on a phase-sample starts the host stack
							// against a partner that swings away ~50ms later.
							// Keep Rp presented and require a full debounce worth
							// of continuous Rd; allow up to 1s for the partner's
							// toggle to come around and commit.
							uint8_t stable = 1;
							for (auto t = 0; t < 100 && stable < 15; t++) {
								HAL_Delay(10);
								Status0 probe{read<Status0>()};
								stable = (probe.BCLevel == 1 || probe.BCLevel == 2) ? stable + 1 : 0;
							}
							if (stable >= 15) {
								pr_debug("Partner Rd committed on CC%d, attaching as host\n", cc2 ? 2 : 1);
								// VBUS is partner-backfed (this whole branch):
								// detach on VBUS loss
								host_partner_sources_vbus = true;
								state = ConnectedState::AsHost;
							} else {
								pr_debug("Partner Rd did not commit, re-polling\n");
								start_toggle_polling(last_polling_mode);
							}
						} else {
							// Neither Rp nor Rd anywhere despite VBUS: transient
							// (partner mid-plug or mid-power-up). Re-arm the toggle
							// and keep polling; state stays TogglePolling.
							start_toggle_polling(last_polling_mode);
						}
					}
				}

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
				// CRCCheck also unmasked: INT_N must assert on each received PD
				// packet so the PD engine can respond within tSenderResponse
				if (state == ConnectedState::AsDevice)
					write<Mask>({.HostCurrentReq = 0,
								 .Collision = 1,
								 .Wake = 1,
								 .Alert = 1,
								 .CRCCheck = 0,
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

				// PD protocol servicing (uses the just-read InterruptA flags)
				pd.service(intrA);

				Status0 status0{read<Status0>()};

				// Look for unplug event:
				// VBusOK = 0 means no VBUS, BCLevel == 0 means CC detected as low (no
				// host pull-up detected).
				//
				// Debounced: some partners (OXI One "Device Self Powered") briefly dip
				// VBUS/CC shortly after attaching. One bad read is not an unplug --
				// treating it as one tears down the connection and restarts the whole
				// attach (and data-role-fallback) sequence, which is where the
				// "works sometimes" behavior came from. Only detach after the link has
				// read down for LinkDownDebounceMs. Persistent outages are re-read by
				// the ~250ms link poll backstop in UsbManager, so a real unplug is
				// confirmed in roughly LinkDownDebounceMs + one poll period. A gap of
				// more than LinkDownEpisodeMs between bad reads starts a new episode
				// (a good read in between isn't guaranteed to reach us: the backstop
				// only calls in when *it* sees a bad status).
				if (status0.VBusOK == 0 || status0.BCLevel == 0) {
					if (!link_down_debounce_enabled && status0.VBusOK && status0.BCLevel == 0) {
						// CC dropped while VBUS stays, before enumeration: not an
						// unplug (a real unplug drops both) -- the partner is a
						// toggling self-powered DRP (OXI One "Device Self
						// Powered"). It will never enumerate us and never holds a
						// CC role, but it latches its device persona on seeing
						// steady Rp. Flip to the source persona and host it.
						auto sw = read<Switches0>();
						pr_debug("CC dropped, VBUS present: toggling self-powered partner; presenting Rp to host it\n");
						attach_as_static_src(sw.MeasureCC2);
						break;
					}
					if (!link_down_debounce_enabled) {
						// Not yet enumerated: treat the dip as a real detach right
						// away (see link_down_debounce_enabled)
						pr_debug("Device link down (Status0=0x%x VBusOK=%d BCLevel=%d), detaching\n",
								 (uint8_t)status0,
								 status0.VBusOK,
								 status0.BCLevel);
						state = ConnectedState::None;
						break;
					}

					auto now = HAL_GetTick();
					if (link_down_since == 0 || now - link_down_last_seen > LinkDownEpisodeMs) {
						link_down_since = now ? now : 1;
						pr_debug("Device link down (Status0=0x%x VBusOK=%d BCLevel=%d), debouncing\n",
								 (uint8_t)status0,
								 status0.VBusOK,
								 status0.BCLevel);
					} else if (now - link_down_since >= LinkDownDebounceMs) {
						pr_debug("Device link down confirmed (Status0=0x%x VBusOK=%d BCLevel=%d)\n",
								 (uint8_t)status0,
								 status0.VBusOK,
								 status0.BCLevel);
						link_down_since = 0;
						state = ConnectedState::None;
					}
					link_down_last_seen = now;
				} else {
					if (link_down_since)
						pr_debug("Device link restored (Status0=0x%x), ignoring dip\n", (uint8_t)status0);
					link_down_since = 0;
				}
			} break;

			case ConnectedState::AsHost: {
				pr_debug("State is currently Host\n");
				Status0 status{read<Status0>()};

				// Partner sources VBUS (toggling self-powered DRP / gadget rig):
				// VBUS-loss is the only trustworthy detach signal -- the OXI One
				// keeps toggling its CC even while enumerated, so BC_LVL flaps
				// between Rd and open forever and means nothing.
				if (host_partner_sources_vbus) {
					if (status.VBusOK == 0) {
						pr_debug("Partner VBUS gone, detaching\n");
						state = ConnectedState::None;
					}
					break;
				}

				// Look for Unplug event:
				// Comp==1, BC==3 means CC pin is read as > 1.23V, meaning no device Rd
				// pull-down
				// FIXME: why isn't Comp set at this point? it gets set a moment later...
				if (/*status.Comp == 1 &&*/ status.BCLevel == 3) {
					// Confirm before detaching: the open reading must out-last a
					// DRP toggle's Rp phase (~30-70ms) -- a partner still mid-
					// commit (OXI One) swings its Rd away and back, and single CC
					// reads here have a history of transients (see FIXME above).
					// A real unplug reads open throughout; the 150ms of extra
					// detach latency is imperceptible.
					bool detached = true;
					for (auto i = 0; i < 15; i++) {
						HAL_Delay(10);
						Status0 confirm{read<Status0>()};
						if (confirm.BCLevel != 3) {
							pr_debug("Host CC open was transient (BCLevel=%d), staying attached\n", confirm.BCLevel);
							detached = false;
							break;
						}
					}
					if (detached)
						state = ConnectedState::None;
				}
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
