#pragma once
#include "i2c.hh"
#include "stm32xx.h"
#include <span>

// Debugging:
//#define MC24C_DEBUG

#ifdef MC24C_DEBUG
#include <cstdio>
#define MC24C_debug printf
#else
static inline void MC24C_debug(...) {
}
#endif

namespace mdrivlib
{

struct M24C_EEPROM {
	M24C_EEPROM(I2CPeriph &i2c, uint8_t addr)
		: i2c{i2c}
		, device_addr(addr << 1) {
	}

	bool read(std::span<uint8_t> data, uint8_t start_addr = 0) {
		auto err = i2c.mem_read(device_addr, start_addr, I2C_MEMADD_SIZE_8BIT, data.data(), data.size());

		MC24C_debug("Read %u bytes from addr %u: err=%d\n", data.size(), start_addr, err);
		for (auto b : data)
			MC24C_debug("%02x ", b);
		MC24C_debug("\n");

		return err == I2CPeriph::I2C_NO_ERR;
	}

	bool read(uint8_t start_addr, std::span<uint8_t> data) {
		return read(data, start_addr);
	}

	bool write(std::span<uint8_t> data, uint8_t start_addr = 0) {
		auto remaining_bytes = data.size();
		auto offset = 0u;
		while (remaining_bytes) {
			auto bytes_to_write =
				std::min(16u, remaining_bytes); //page size: datasheet says cannot write more than 16 bytes a time

			auto page = data.subspan(offset, bytes_to_write);
			auto err =
				i2c.mem_write(device_addr, start_addr + offset, I2C_MEMADD_SIZE_8BIT, page.data(), bytes_to_write);

			HAL_Delay(6); // Datasheet: tWR: worst-case write time = 5ms

			MC24C_debug("Wrote %u bytes to addr %u: err=%d\n", bytes_to_write, start_addr + offset, err);
			for (auto b : page)
				MC24C_debug("%02x ", b);
			MC24C_debug("\n");

			remaining_bytes -= bytes_to_write;
			offset += bytes_to_write;

			if (err != I2CPeriph::I2C_NO_ERR)
				return false;
		}

		return true;
	}

	bool write(auto data, uint8_t start_addr = 0) {
		return write(std::span<uint8_t>{reinterpret_cast<uint8_t *>(&data), sizeof(data)}, start_addr);
	}

	I2CPeriph &i2c;
	uint8_t device_addr;
};

} // namespace mdrivlib
