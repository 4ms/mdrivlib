#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{
namespace stm32mp1
{
namespace core_a7
{
namespace SystemCache
{
inline void invalidate_dcache() {
	L1C_InvalidateDCacheAll();
}

template<typename ptr>
inline void invalidate_dcache_by_addr(ptr addr) {
	L1C_InvalidateDCacheMVA(reinterpret_cast<void *>(addr));
}

inline void invalidate_dcache_by_range(void *addr, int32_t size) {
	uint32_t *u32_ptr = reinterpret_cast<uint32_t *>(addr);
	while (size > 0) {
		L1C_InvalidateDCacheMVA(u32_ptr);
		u32_ptr += 1;
		size -= 4;
	}
}

inline void clean_dcache() {
	L1C_CleanDCacheAll();
}

template<typename ptr>
inline void clean_dcache_by_addr(ptr addr) {
	L1C_CleanDCacheMVA(reinterpret_cast<void *>(addr));
}

inline void clean_dcache_by_range(void *addr, int32_t size) {
	uint32_t *u32_ptr = reinterpret_cast<uint32_t *>(addr);
	while (size > 0) {
		L1C_CleanDCacheMVA(u32_ptr);
		u32_ptr += 1;
		size -= 4;
	}
}

inline void clean_and_invalidate_dcache_by_range(void *addr, int32_t size) {
	uint32_t *u32_ptr = reinterpret_cast<uint32_t *>(addr);
	while (size > 0) {
		L1C_CleanInvalidateDCacheMVA(u32_ptr);
		u32_ptr += 1;
		size -= 4;
	}
}
} // namespace SystemCache
} // namespace core_a7
} // namespace stm32mp1
} // namespace mdrivlib
