#pragma once
#include "drivers/i2c.hh"
#include "util/zip.hh"
#include <string_view>

namespace MetaModule
{

class BatteryGuage {
	using I2CPeriph = mdrivlib::I2CPeriph;

public:
	BatteryGuage(mdrivlib::I2CConfig const &config, uint8_t I2C_address)
		: i2c{config}
		, address{I2C_address} {
		i2c.enable_IT(config.priority1, config.priority2);
	}

	void init() {
	}

	void update() {
		if (!i2c.is_ready())
			return;

		if (HAL_GetTick() - last_read_tm > 1000) {
			last_read_tm = HAL_GetTick();

			for (auto [reg, state] : zip(registers, states)) {
				auto newval = read_reg(reg).value_or(state);
				if (newval != state) {
					state = newval;
				}
			}
		}
	}

	void dump_registers() {
		HAL_Delay(100);
		uint8_t val[3];

		val[0] = 0x00;
		val[1] = 0x01;
		val[2] = 0x00;

		// Control()
		if (auto err = i2c.write(address, val, 2); err != I2CPeriph::Error::I2C_NO_ERR)
			printf("Write ERR %d\n", err);

		if (auto err = i2c.read(address, val, 2); err == I2CPeriph::Error::I2C_NO_ERR)
			printf("Device ID: %02x %02x\n", val[0], val[1]);
		else
			printf("Read ERR %d\n", err);

		for (auto reg : registers) {
			if (auto st = read_reg(reg))
				printf("%s: %d\n", reg.name.data(), *st);
			else
				printf("%s: error reading\n", reg.name.data());
		}
	}

	uint16_t battery_percent_remaining() const {
		return states[11]; // State of Charge
	}

	// bool is_charging() const {
	// }

private:
	I2CPeriph i2c{};
	uint8_t address{0xaa};
	uint32_t last_read_tm = 0;

	struct Registers {
		uint8_t addr;
		std::string_view name;
		std::string_view units;
	};

	static constexpr std::array<Registers, 19> registers{{
		{0x02, "Temperature", "0.1K"}, //0
		{0x04, "Voltage", "mV"},
		{0x06, "Flags", ""},
		{0x08, "NominalAvailableCapacity", "mAh"},
		{0x0A, "FullAvailableCapacity", "mAh"},
		{0x0C, "RemainingCapacity", "mAh"},
		{0x0E, "FullChargeCapacity", "mAh"},
		{0x10, "AverageCurrent", "mA"},
		{0x12, "StandbyCurrent", "mA"},
		{0x14, "MaxLoadCurrent", "mA"},
		{0x18, "AveragePower", "mAh?"},
		{0x1C, "StateOfCharge", "% or bits"}, //11
		{0x1E, "InternalTemperature", "0.1K"},
		{0x20, "StateOfHealth", "% or bits"},
		{0x28, "RemainingCapacityUnfiltered", "mAh"},
		{0x2A, "RemainingCapacityFiltered", "mAh"},
		{0x2C, "FullChargeCapacityUnfiltered", "mAh"},
		{0x2E, "FullChargeCapacityFiltered", "mAh"},
		{0x30, "StateOfChargeUnfiltered", "% or bits"},
	}};

	std::array<int16_t, 19> states{};

	enum FlagBits {
		OverTemperature = (1 << 15),
		UnderTemperature = (1 << 14),
		FullChargeDetected = (1 << 9),
		Charging = (1 << 8),
		OCVTAKEN = (1 << 7),
		PowerOnOrResetEvent = (1 << 5),
		ConfigUpdateMode = (1 << 4),
		BatteryDetected = (1 << 3),
		SOCLessThanThreshold1 = (1 << 2),
		SOCLessThanThresholdF = (1 << 1),
		Discharging = (1 << 0),
	};

	std::optional<int16_t> read_reg(uint8_t reg) {
		uint8_t data[2];
		auto err = i2c.mem_read(address, reg, I2C_MEMADD_SIZE_8BIT, data, 2);
		if (err == I2CPeriph::Error::I2C_NO_ERR)
			return ((uint16_t)data[1] << 8) | data[0];
		else
			return {};
	}
	std::optional<int16_t> read_reg(Registers reg) {
		return read_reg(reg.addr);
	}
};

} // namespace MetaModule
