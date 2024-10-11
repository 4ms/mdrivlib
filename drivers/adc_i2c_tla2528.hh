#pragma once
#include "adc_i2c_tla2528_registers.hh"
#include "i2c.hh"
#include "pin.hh"
#include "stm32xx.h"
#include "util/bitfield.hh"

// Debugging:
#define TLA2528_DEBUG
#ifdef TLA2528_DEBUG
#include <cstdio>
#define tla2528_debug printf
#else
static inline void tla2528_debug(...) {
}
#endif

namespace mdrivlib::TLA2528
{

struct Config {
	static constexpr uint8_t LowestAllowedAddr = 0x10;
	static constexpr uint8_t HighestAllowedAddr = 0x17;

	uint8_t addr{};
	bool oversample = false;
	uint16_t irq_priority = 3;
	uint16_t irq_subpriority = 3;
	// pins can be analog in (default) or GPIO in or out. TODO Add config for this
};

class Device {
public:
	Device(I2CPeriph &i2c, const Config &conf)
		: _device_addr(conf.addr << 1)
		, _i2c{i2c} {
		i2c.enable_IT(conf.irq_priority, conf.irq_subpriority);
	}

	auto start() {
		// TODO: Calibrate?
		// Enable oversampling: OSR[2:0] bits in the OSR_CFG

		// Set sampling rate with CLK_DIV and OSC_SEL
		// Auto sequence: AUTO_SEQ_CHSEL, SEQ_MODE = 1, SEQ_START = 1
		return Error::None;
	}

	bool is_present() {
		auto val = read<SystemStatus>();
		return (val.RsvdAlwaysHigh == 1);
	}

	void set_address(uint8_t dev_addr_unshifted) {
		_device_addr = dev_addr_unshifted << 1;
	}

	enum Error {
		None,
		WriteConfigFailed,
		InvalidChannelID,
		ReadChannelFailed,
	};

private:
	uint8_t _device_addr;
	I2CPeriph &_i2c;

	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		auto err = _i2c.write_reg(_device_addr, data);
		if (err == mdrivlib::I2CPeriph::I2C_NO_ERR)
			return true;
		else {
			tla2528_debug("Failed to write to reg 0x%x\n", Reg::Address);
			return false;
		}
	}

	template<typename Reg>
	Reg read() {
		static bool got_error_once = false;
		auto reg = _i2c.read_reg<Reg>(_device_addr);

		if (!reg.has_value() && !got_error_once) {
			tla2528_debug("TLA2528: Error reading Reg 0x%x\n", Reg::Address);
			got_error_once = true;
		}
		return reg.value_or(Reg::make(0));
	}
};

} // namespace mdrivlib::TLA2528
