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
	static constexpr bool oversample_append_channel = false;

public:
	Device(I2CPeriph &i2c, const Config &conf)
		: _i2c{i2c}
		, _device_addr(conf.addr << 1)
		, _oversample{conf.oversample} {
		i2c.enable_IT(conf.irq_priority, conf.irq_subpriority);
	}

	bool start() {
		// TODO: Calibrate mode
		bool ok = true;
		ok &= write(GeneralCfg{.Reset = 1, .ForceChannelsAnalog = 1});
		ok &= write(OpMode{.ClockDiv = 0, .OscSel = OpMode::OscSelHighSpeed});

		ok &= write(PinCfg{.BitMaskDigitalGPIO = 0});
		ok &= write(SequenceCfg{.SeqModeAuto = 1});
		ok &= write(AutoSeqChanSel{.BitMaskChannelEnabled = 0xFF});

		if (_oversample) {
			ok &= write(OversampleCfg{.OversampleBits = 0b111});
			if constexpr (oversample_append_channel)
				ok &= write(DataCfg{.AppendStatus = 1});
			else
				ok &= write(DataCfg{.AppendStatus = 0});
		} else {
			ok &= write(OversampleCfg{.OversampleBits = 0b000});
			ok &= write(DataCfg{.AppendStatus = 1});
		}

		// Start conversion:
		ok &= write(SequenceCfg{.SeqModeAuto = 1, .SeqStart = 1});
		return ok;
	}

	bool is_present() const {
		auto [val, ok] = read<SystemStatus>();
		return ok && (val.RsvdAlwaysHigh == 1);
	}

	bool read_channels() {
		// no oversampling: 1.6ms
		// oversampling with chan_id appended: 3.3ms
		// oversampling with no chan_id: 2.6ms
		auto num_bytes = _oversample && oversample_append_channel ? 24 : 16;
		auto err = _i2c.read_IT(_device_addr, _data, num_bytes);
		chan_ctr = 0;
		return err == I2CPeriph::I2C_NO_ERR;
	}

	struct Reading {
		uint16_t value;
		uint8_t chan;
	};

	Reading collect_reading(uint32_t chan) {
		if (_oversample) {
			if constexpr (oversample_append_channel)
				return process_16bit(_data[chan * 3], _data[chan * 3 + 1], _data[chan * 3 + 2]);
			else
				return process_16bit(_data[chan * 2], _data[chan * 2 + 1], chan_ctr++);

		} else
			return process_12bit(_data[chan * 2], _data[chan * 2 + 1]);
	}

	void set_address(uint8_t dev_addr_unshifted) {
		_device_addr = dev_addr_unshifted << 1;
	}

private:
	I2CPeriph &_i2c;
	uint8_t _device_addr;
	bool _oversample;
	uint8_t chan_ctr = 0;

	uint8_t _data[oversample_append_channel ? 24 : 16]{};

	static Reading process_12bit(uint8_t data0, uint8_t data1) {
		//Top 12 bits are the reading, bottom 4 bits are the channel number
		uint8_t chan = data1 & 0b1111;
		uint16_t value = (uint16_t(data0) << 4) | (data1 >> 4);
		return Reading{.value = value, .chan = chan};
	}

	static Reading process_16bit(uint8_t data0, uint8_t data1, uint8_t chan) {
		// Scale and round 16-bit value to 12-bit value
		uint16_t value = (uint16_t(data0) << 4) | (data1 >> 4);
		if (data1 & 0b1000)
			value++;
		return Reading{.value = value, .chan = chan};
	}

	// Returns true on success
	template<typename Reg>
	requires std::derived_from<Reg, BusReg::WriteAccess>
	bool write(Reg reg_value) {
		// TLA2528 has specific way of writing a register:
		// Write the "Write opcode", then register addr, then register data
		uint8_t bytes[3] = {Operation::WriteReg, Reg::Address, uint8_t(reg_value)};
		if (auto err = _i2c.write(_device_addr, bytes, 3); err == mdrivlib::I2CPeriph::I2C_NO_ERR) {
			return true;
		} else {
			tla2528_debug("TLA2528: Error writing reg 0x%x to device 0x%x\n", Reg::Address, _device_addr);
			return false;
		}
	}

	template<typename Reg>
	requires std::derived_from<Reg, BusReg::ReadAccess>
	std::pair<Reg, bool> read() const {
		// TLA2528 has specific way of reading a register:
		// 1) Write the "Read opcode" and register addr
		uint8_t data[2] = {Operation::ReadReg, Reg::Address};
		if (auto err = _i2c.write(_device_addr, data, 2); err == I2CPeriph::I2C_NO_ERR) {

			// 2) Read register contents (1 byte)
			if (auto err = _i2c.read(_device_addr, data, 1); err == I2CPeriph::I2C_NO_ERR)
				return {Reg::make(data[0]), true};
		}

		// Failure:
		tla2528_debug("TLA2528: Error reading reg 0x%x from device 0x%x\n", Reg::Address, _device_addr);
		return {Reg::make(0), false};
	}
};

} // namespace mdrivlib::TLA2528
