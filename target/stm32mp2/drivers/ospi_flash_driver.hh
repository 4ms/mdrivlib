#pragma once
#include "drivers/ospi_flash_struct.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

// Serial NOR flash (Winbond W25Qxx command set) on the STM32MP2 OCTOSPI, in indirect mode.
//
// Everything runs in the foreground: the EXECUTE_BACKGROUND options are accepted, but block like
// EXECUTE_FOREGROUND.
//
// Only one core may use the flash at a time: the constructor resets and re-initializes the OCTOSPI.
class OSpiFlash {
public:
	enum ErasableSizes : uint32_t {
		SECTOR = 0x1000,
		BLOCK_32K = 0x8000,
		BLOCK_64K = 0x10000,
		ENTIRE_CHIP = 0xFFFFFFFF
	};
	enum UseInterruptFlags { EXECUTE_FOREGROUND, EXECUTE_BACKGROUND };

	static constexpr uint32_t PageSize = 256;

	OSpiFlash(const OSPIFlashConfig &config);

	bool is_ready() {
		return true;
	}

	bool read(uint8_t *data, uint32_t read_addr, uint32_t num_bytes, UseInterruptFlags = EXECUTE_FOREGROUND);
	bool read_background(uint8_t *data, uint32_t read_addr, uint32_t num_bytes) {
		return read(data, read_addr, num_bytes);
	}

	// Writes any number of bytes, splitting at page boundaries. The flash must be erased first.
	bool write(const uint8_t *data, uint32_t write_addr, uint32_t num_bytes);

	// Writes within one page (256 bytes)
	bool
	write_page(const uint8_t *data, uint32_t write_addr, uint32_t num_bytes, UseInterruptFlags = EXECUTE_FOREGROUND);

	// size is one of the ErasableSizes. base_addr is rounded down to a multiple of size.
	bool erase(uint32_t size, uint32_t base_addr, UseInterruptFlags = EXECUTE_FOREGROUND);
	bool erase_background(ErasableSizes size, uint32_t base_addr) {
		return erase(size, base_addr);
	}
	bool erase_block_background(uint32_t base_addr) {
		return erase(BLOCK_32K, base_addr);
	}

	// JEDEC ID: manufacturer in bits 0-7, memory type 8-15, capacity 16-23 (W25Q128JV: 0x1840EF)
	bool read_chip_id(uint32_t *chip_id);

	// Attempts a few times to read the ID, returns true if (id & mask) == expected_id
	bool check_chip_id(uint32_t expected_id, uint32_t mask);

	uint32_t get_chip_size_bytes() {
		return defs.flash_size_bytes;
	}

	bool reset();

private:
	OSPI_HandleTypeDef handle{};
	const OSPIFlashConfig &defs;
	bool quad_mode = false;

	OSPI_RegularCmdTypeDef command(uint32_t instruction);
	bool send(OSPI_RegularCmdTypeDef &cmd, uint32_t timeout = HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	bool read_register(uint32_t instruction, uint8_t *value);
	bool write_enable();
	bool wait_until_ready(uint32_t timeout);
	bool enable_quad_mode();
};

} // namespace mdrivlib
