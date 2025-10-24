#pragma once
#include "drivers/i2c.hh"
#include "drivers/i2c_config_struct.hh"
#include <cstdint>

namespace mdrivlib
{

struct LIS2HH12accelerometer {
	// Unshifted 7-bit addresses:
	static constexpr uint8_t base_address_0 = 0b001'1110; // SA0 connected to gnd
	static constexpr uint8_t base_address_1 = 0b001'1101; // SA0 connected to v+

	struct Registers {

		static constexpr uint8_t temp_L = 0x0b;
		static constexpr uint8_t temp_H = 0x0c;

		struct ID : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x0f;
			uint8_t id;
			static ID make(uint8_t raw) {
				return ID{.id = raw};
			}
			static constexpr uint8_t whoAmIRespsonse = 0x41;
		};

		struct CTRL1 : BusReg::ReadWrite {
			static constexpr uint8_t Address = 0x20;
			uint8_t x_enable : 1;
			uint8_t y_enable : 1;
			uint8_t z_enable : 1;
			uint8_t block_data_update : 1;
			uint8_t out_data_rate : 3;
			uint8_t hires : 1;

			constexpr operator uint8_t() {
				return (x_enable << 0) | (y_enable << 1) | (z_enable << 2) | (block_data_update << 3) |
					   (out_data_rate << 4) | (hires < 7);
			}

			constexpr static auto make(uint8_t raw) {
				return CTRL1{
					.x_enable = BusReg::Bits<0>(raw),
					.y_enable = BusReg::Bits<1>(raw),
					.z_enable = BusReg::Bits<2>(raw),
					.block_data_update = BusReg::Bits<3>(raw),
					.out_data_rate = BusReg::Bits<4, 5, 6>(raw),
					.hires = BusReg::Bits<7>(raw),
				};
			}
		};

		static constexpr uint8_t outX_L = 0x28;
		static constexpr uint8_t outX_H = 0x29;
		static constexpr uint8_t outY_L = 0x2a;
		static constexpr uint8_t outY_H = 0x2b;
		static constexpr uint8_t outZ_L = 0x2c;
		static constexpr uint8_t outZ_H = 0x2d;

		struct outXL : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x28;
			uint8_t val;
			static auto make(uint8_t raw) {
				return outXL{.val = raw};
			}
		};
		struct outXH : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x29;
			uint8_t val;
			static auto make(uint8_t raw) {
				return outXH{.val = raw};
			}
		};

		struct outYL : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x2a;
			uint8_t val;
			static auto make(uint8_t raw) {
				return outYL{.val = raw};
			}
		};
		struct outYH : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x2b;
			uint8_t val;
			static auto make(uint8_t raw) {
				return outYH{.val = raw};
			}
		};

		struct outZL : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x2c;
			uint8_t val;
			static auto make(uint8_t raw) {
				return outZL{.val = raw};
			}
		};
		struct outZH : BusReg::ReadOnly {
			static constexpr uint8_t Address = 0x2d;
			uint8_t val;
			static auto make(uint8_t raw) {
				return outZH{.val = raw};
			}
		};
	};

	LIS2HH12accelerometer(I2CConfig const &i2c_conf)
		: i2c{i2c_conf}
		, dev_addr{base_address_0 << 1} {
		i2c.enable_IT(i2c_conf.priority1, i2c_conf.priority2);
	}

	void init() {
		if (auto id = i2c.read_reg<Registers::ID>(dev_addr)) {
			if (id->id == Registers::ID::whoAmIRespsonse) {
				i2c.write_reg(dev_addr,
							  Registers::CTRL1{
								  .x_enable = 1,
								  .y_enable = 1,
								  .z_enable = 1,
								  .block_data_update = 1,
								  .out_data_rate = 0b110,
								  .hires = 0,
							  });
			} else {
				printf("Chip found at i2s address %x, but not a LIS2HH12\n", dev_addr);
			}

		} else
			printf("Can't read LIS2HH12 ID register\n");
	}

	struct XYZ {
		int16_t x{};
		int16_t y{};
		int16_t z{};
	};

	void update() {
		static XYZ xyz{};

		if (i2c.is_ready()) {
			auto res =
				i2c.mem_read_IT(dev_addr, Registers::outXL::Address, I2C_ADDRESSINGMODE_7BIT, (uint8_t *)(&xyz), 6);

			if (res == I2CPeriph::I2C_NO_ERR) {
				last_reading = xyz;
			}
		}
	}

	XYZ get_latest() {
		return last_reading;
	}

	XYZ read_xyz() {
		XYZ xyz;
		xyz.x = read_x();
		xyz.y = read_y();
		xyz.z = read_z();
		return xyz;
	}

	int16_t read_x() {
		auto l = i2c.read_reg<Registers::outXL>(dev_addr);
		auto h = i2c.read_reg<Registers::outXH>(dev_addr);
		if (l && h)
			return l->val | (h->val << 8);
		else
			return 0;
	}

	int16_t read_y() {
		auto l = i2c.read_reg<Registers::outYL>(dev_addr);
		auto h = i2c.read_reg<Registers::outYH>(dev_addr);
		if (l && h)
			return l->val | (h->val << 8);
		else
			return 0;
	}

	int16_t read_z() {
		auto l = i2c.read_reg<Registers::outZL>(dev_addr);
		auto h = i2c.read_reg<Registers::outZH>(dev_addr);
		if (l && h)
			return l->val | (h->val << 8);
		else
			return 0;
	}

private:
	I2CPeriph i2c;
	const uint8_t dev_addr;

	XYZ last_reading{};
};
} // namespace mdrivlib
