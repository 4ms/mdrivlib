#pragma once
#include "drivers/stm32xx.h"
#include <cstdint>

// Data cache for the Cortex-M33: the DCACHE peripheral on the S-AHB bus (DDR and other
// non-code-region memories). Same API as the A35's cache.hh so shared drivers compile unchanged.
//
// What gets cached is set by the MPU's memory attributes, so configure the MPU before
// enable_dcache(), and make any memory shared with the A35 non-cacheable.
// The maintenance functions do nothing while the cache is disabled.
//
// The code-region (C-AHB) instruction cache, ICACHE, is separate and not handled here.

namespace mdrivlib::SystemCache
{

inline DCACHE_HandleTypeDef &dcache_handle() {
	static DCACHE_HandleTypeDef handle{};
	return handle;
}

inline bool is_dcache_enabled() {
	return DCACHE->CR & DCACHE_CR_EN;
}

inline bool enable_dcache() {
	auto &h = dcache_handle();
	h.Instance = DCACHE;
	h.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
	if (HAL_DCACHE_Init(&h) != HAL_OK)
		return false;
	return HAL_DCACHE_Enable(&h) == HAL_OK;
}

inline void mem_barrier() {
	__DMB();
}

inline void invalidate_dcache() {
	if (is_dcache_enabled())
		HAL_DCACHE_Invalidate(&dcache_handle());
}

inline void invalidate_dcache_by_range(void *addr, uint32_t size) {
	if (is_dcache_enabled())
		HAL_DCACHE_InvalidateByAddr(&dcache_handle(), reinterpret_cast<const uint32_t *>(addr), size);
}

template<typename ptr>
inline void invalidate_dcache_by_addr(ptr addr) {
	invalidate_dcache_by_range((void *)addr, 4);
}

inline void clean_dcache_by_range(void *addr, uint32_t size) {
	if (is_dcache_enabled())
		HAL_DCACHE_CleanByAddr(&dcache_handle(), reinterpret_cast<const uint32_t *>(addr), size);
}

template<typename ptr>
inline void clean_dcache_by_addr(ptr addr) {
	clean_dcache_by_range((void *)addr, 4);
}

inline void clean_and_invalidate_dcache_by_range(void *addr, int32_t size) {
	if (is_dcache_enabled())
		HAL_DCACHE_CleanInvalidByAddr(&dcache_handle(), reinterpret_cast<const uint32_t *>(addr), size);
}

// No whole-cache clean command: use the by-range functions
inline void clean_dcache() {
}

} // namespace mdrivlib::SystemCache
