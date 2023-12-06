#pragma once
#include <cstdint>
#include <span>

namespace mdrivlib
{

struct InternalFlash {
	static bool read(std::span<uint32_t> data, uint32_t address);
	static bool erase_sector(uint32_t address);
	static bool write(std::span<const uint32_t> data, uint32_t address);
};

} // namespace mdrivlib
