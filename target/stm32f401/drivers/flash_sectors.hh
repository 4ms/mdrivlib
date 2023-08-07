#pragma once
#include <cstdint>

constexpr uint32_t NumFlashSectors = 8;

constexpr uint32_t get_sector_addr(uint32_t sector_num) {
	//F401xD/E
	constexpr uint32_t F401xE_SECTORS[] = {
		0x08000000, // Sector 0: 16k
		0x08004000, // Sector 1: 16k
		0x08008000, // Sector 2: 16k
		0x0800C000, // Sector 3: 16k
		0x08010000, // Sector 4: 64k
		0x08020000, // Sector 5: 128k
		0x08040000, // Sector 6: 128k
		0x08060000, // Sector 7: 128k
		0x08080000, // end
	};
	return (sector_num <= NumFlashSectors) ? F401xE_SECTORS[sector_num] : 0;
}
