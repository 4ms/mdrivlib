#pragma once
extern "C" {
#include "core_ca.h"
}

namespace mdrivlib
{
namespace stm32mp1
{
namespace corea7
{
struct SystemCache {
	void invalidate_dcache() {
		L1C_InvalidateDCacheAll();
	}

	void invalidate_dcache_by_addr(void *addr) {
		L1C_InvalidateDCacheMVA(addr);
	}

	void invalidate_dcache_by_range(void *addr, int32_t size) {
		uint32_t *u32_ptr = reinterpret_cast<uint32_t *>(addr);
		while (size > 0) {
			L1C_InvalidateDCacheMVA(u32_ptr);
			u32_ptr += 4;
			size -= 4;
		}
	}

	void clean_dcache() {
		L1C_CleanDCacheAll();
	}

	void clean_dcache_by_addr(void *addr) {
		L1C_CleanDCacheMVA(addr);
	}

	void clean_dcache_by_range(void *addr, uint32_t size) {
		uint32_t *u32_ptr = reinterpret_cast<uint32_t *>(addr);
		while (size > 0) {
			L1C_CleanDCacheMVA(u32_ptr);
			u32_ptr += 4;
			size -= 4;
		}
	}
};
} // namespace corea7
} // namespace stm32mp1
} // namespace mdrivlib
