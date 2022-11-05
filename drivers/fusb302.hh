#pragma once
#include "drivers/i2c.hh"
#include "fusb302_registers.hh"
#include <cstdint>

//Debugging:
#define DEBUG
#ifdef DEBUG
#include "printf.h"
#define pr_debug printf_
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

	void start_drp_polling() {
		// Setup per datasheet p. 7 (Toggle Functionality)
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 0});

		// Clear interrupts
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();

		// Turn toggle bit off, then on (otherwise TOGGLE mode is not re-started)
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		HAL_Delay(10);
		write<Control2>({.Toggle = 1, .PollingMode = Control2::PollDRP, .ToggleIgnoreRa = 1});

		write<Switches0>({.ConnectVConnCC1 = 0, .ConnectVConnCC2 = 0});
		//Note: setting Mask::VBusOK to 0 when HostCurrentReq is 0 results in it not detecting disconnect as host
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
		pr_debug("IntA = 0x%x TogDone=%d\n", intrA, intrA.ToggleDone);

		//TODO: update state based on interrupt register contents
		switch (state) {
			case ConnectedState::TogglePolling: {
				pr_debug("State is currently Polling\n");

				Status1A status1a{read<Status1A>()};
				if (status1a.ToggleOutcomeIsSink)
					state = ConnectedState::AsDevice;

				else if (status1a.ToggleOutcomeIsCC1 || status1a.ToggleOutcomeIsCC2)
					state = ConnectedState::AsHost;

				//could also check Status0: Comp == 0 && BCLevel < 3
				//Comp == 0 means CC pin is read as less than reference, meaning device Rd pull-down was detected
				//BC<3 means CC pin is read as < 1.23V, meaning a device Rd pull-down was detected
			} break;

			case ConnectedState::AsDevice: {
				pr_debug("State is currently Device\n");
				Status0 status0{read<Status0>()};

				// Look for unplug event:
				// VBusOK = 0 means no VBUS, BCLevel == 0 means CC detected as low (no host pull-up detected)
				if (status0.VBusOK == 0 && status0.BCLevel == 0)
					state = ConnectedState::None;
			} break;

			case ConnectedState::AsHost: {
				pr_debug("State is currently Host\n");
				Status0 status{read<Status0>()};
				// Look for Unplug event:
				// Comp==1, BC==3 means CC pin is read as > 1.23V, meaning no device Rd pull-down
				//FIXME: why isn't Comp set at this point? it gets set a moment later...
				if (/*status.Comp == 1 &&*/ status.BCLevel == 3)
					state = ConnectedState::None;
			} break;

			case ConnectedState::None: {
				pr_debug("State is currently None\n");
			} break;
		}
	}

	// Returns true on sucess
	template<typename Reg>
		requires std::derived_from<Reg, BusReg::WriteAccess> bool
	write(Reg data) {
		auto d = static_cast<uint8_t>(data);
		return i2c.mem_write(dev_addr, Reg::Address, 1, &d, 1) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	//TODO: return std::expected<uint8_t> when compiler supports it
	template<typename Reg>
		requires std::derived_from<Reg, BusReg::ReadAccess>
	Reg read() {
		static bool got_error_once = false;
		uint8_t data = 0;
		auto err = i2c.mem_read(dev_addr, Reg::Address, 1, &data, 1);

		if (err != mdrivlib::I2CPeriph::I2C_NO_ERR && !got_error_once) {
			pr_debug("Error reading Reg 0x%x: %d\n", Reg::Address, err);
			got_error_once = true;
		}
		return Reg::make(data);
	}

	template<typename Reg>
		requires std::derived_from<Reg, BusReg::ReadAccess>
	void reg_check(std::string_view regname) {
		static uint8_t last_val = 0xFF;
		auto val = read<Reg>();
		if ((uint8_t)val != last_val)
			pr_debug("Changed %s: 0x%x\n", regname.data(), val);
		last_val = val;
	}

	template<typename Reg>
		requires std::derived_from<Reg, BusReg::ReadAccess>
	void reg_dump(std::string_view regname) {
		auto val = read<Reg>();
		pr_debug("%s: 0x%x\n", regname.data(), val);
		if constexpr (static_cast<uint8_t>(Reg::Address) == FUSB302::Status0::Address) {
			auto s = Status0::make(val);
			pr_debug("   BCLevel=%d Wake=%d Comp=%d VBusOK=%d\n", s.BCLevel, s.Wake, s.Comp, s.VBusOK);
		}
	}

	void check_all_regs() {
#ifdef DEBUG
		reg_check<ID>("ID");
		reg_check<Switches0>("Switches0");
		reg_check<Switches1>("Switches1");
		reg_check<Measure>("Measure");
		reg_check<Slice>("Slice");
		reg_check<Control0>("Control0");
		reg_check<Control1>("Control1");
		reg_check<Control2>("Control2");
		reg_check<Control3>("Control3");
		reg_check<Mask>("Mask1");
		reg_check<Power>("Power");
		reg_check<OCP>("OCP");
		reg_check<MaskA>("MaskA");
		reg_check<MaskB>("MaskB");
		reg_check<Control4>("Control4");
		reg_check<Status0A>("Status0A");
		reg_check<Status1A>("Status1A");
		reg_check<Status0>("Status0");
		reg_check<Status1>("Status1");
#endif
	}

	void dump_all_regs() {
#ifdef DEBUG
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
#endif
	}
};

} // namespace FUSB302
