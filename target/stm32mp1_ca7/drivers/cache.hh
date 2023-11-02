#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib::SystemCache
{
// Data clean by VMA to point of unification
static inline void __set_DCCMVAU(uint32_t value) {
	__set_CP(15, 0, value, 7, 11, 1);
}

inline void invalidate_dcache() {
	L1C_InvalidateDCacheAll();
}

template<typename ptr>
inline void invalidate_dcache_by_addr(ptr addr) {
	L1C_InvalidateDCacheMVA(reinterpret_cast<void *>(addr));
}

static constexpr uint32_t CacheLineBytes = 64;
static constexpr uint32_t CacheLineMask = ~(CacheLineBytes - 1);

inline void invalidate_dcache_by_range(void *addr, uint32_t size) {
	// starting address of cache line containing the data
	auto start_addr = reinterpret_cast<uint32_t>(addr) & CacheLineMask;

	// starting address of next cache line after end of the data
	auto end_addr = (reinterpret_cast<uint32_t>(addr) + size + CacheLineBytes - 1) & CacheLineMask;

	for (uint32_t addr = start_addr; addr < end_addr; addr += CacheLineBytes) {
		L1C_InvalidateDCacheMVA(reinterpret_cast<void *>(addr));
	}
	__DSB();
}

inline void clean_dcache() {
	L1C_CleanDCacheAll();
}

template<typename ptr>
inline void clean_dcache_by_addr(ptr addr) {
	L1C_CleanDCacheMVA(reinterpret_cast<void *>(addr));
}

inline void clean_dcache_by_range(void *addr, uint32_t size) {
	// starting address of cache line containing the data
	auto start_addr = reinterpret_cast<uint32_t>(addr) & CacheLineMask;

	// starting address of next cache line after end of the data
	auto end_addr = (reinterpret_cast<uint32_t>(addr) + size + CacheLineBytes - 1) & CacheLineMask;

	for (uint32_t addr = start_addr; addr < end_addr; addr += CacheLineBytes) {
		// L1C_CleanDCacheMVA(reinterpret_cast<void *>(addr));
		__set_DCCMVAC(addr);
		__DMB();
	}
	__DSB();

	// TODO: this was used, but seems to be based on 32-byte lines and seems to invalidate each line 8 times
	// Makes sure the new code above works
	// auto addr_line = reinterpret_cast<uint32_t>(addr) & ~0b11111; // just keep Tag and Set/Index
	// while (size > 0) {
	// 	__set_DCCMVAC(addr_line);
	// 	addr_line += 0b100000;
	// 	size -= 4;
	// }
	// __DMB();
}

inline void clean_and_invalidate_dcache_by_range(void *addr, int32_t size) {
	auto *u32_ptr = reinterpret_cast<uint32_t *>(addr);
	while (size > 0) {
		__set_DCCIMVAC((uint32_t)u32_ptr);
		u32_ptr += 1;
		size -= 4;
	}
	__DMB();
}
} // namespace mdrivlib::SystemCache
