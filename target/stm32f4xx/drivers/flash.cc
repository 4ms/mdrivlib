#include "drivers/flash.hh"
#include "drivers/flash_sectors.hh"
#include "drivers/stm32xx.h"
#include <cstdint>
#include <span>

namespace mdrivlib
{
static void _clear_error_codes();
static bool _erase_sector(uint32_t sector);

bool InternalFlash::read(std::span<uint32_t> data, uint32_t address) {
	for (auto &d : data) {
		d = (uint32_t)(*(volatile uint32_t *)address);
		address += 4;
	}
	return true;
}

bool InternalFlash::erase_sector(uint32_t address) {
	_clear_error_codes();
	HAL_FLASH_Unlock();
	for (uint32_t i = 0; i < NumFlashSectors; ++i) {
		if (address == get_sector_addr(i)) {
			return _erase_sector(i);
		}
	}
	HAL_FLASH_Lock();

	return false; // address out of range
}

bool InternalFlash::write(std::span<const uint32_t> data, uint32_t address) {
	_clear_error_codes();
	HAL_FLASH_Unlock();

	for (auto const d : data) {
		if (address >= get_sector_addr(1)) {
			auto status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, d);
			if (status != HAL_OK) {
				HAL_FLASH_Lock();
				return false;
			}
			address += 4;
		}
	}

	HAL_FLASH_Lock();
	return true;
}

static bool _erase_sector(uint32_t sector) {
	FLASH_EraseInitTypeDef erase_conf = {
		.TypeErase = FLASH_TYPEERASE_SECTORS,
		.Banks = FLASH_BANK_1,
		.Sector = sector,
		.NbSectors = 1,
		.VoltageRange = FLASH_VOLTAGE_RANGE_3,
	};

	uint32_t err;
	return HAL_FLASHEx_Erase(&erase_conf, &err) == HAL_OK;
}

static void _clear_error_codes() {
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
#if defined(FLASH_SR_RDERR)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_RDERR);
#endif
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
}

} // namespace mdrivlib
