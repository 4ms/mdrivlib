#pragma once
#include "drivers/system_reg.hh"
#include <cstdint>

// AArch64 cache maintenance for the Cortex-A35.
// Same API as target/stm32mp1_ca7/drivers/cache.hh so shared drivers compile unchanged.

namespace mdrivlib::SystemCache
{

static constexpr uintptr_t CacheLineBytes = 64;
static constexpr uintptr_t CacheLineMask = ~(CacheLineBytes - 1);

inline void mem_barrier() {
	asm volatile("dmb sy" ::: "memory");
}

inline void invalidate_icache() {
	ic_iallu();
	dsb_sy();
	isb();
}

// Whole-cache ops on AArch64 iterate by set/way and clean+invalidate
// (invalidate-only by set/way is unsafe on a running core).
inline void invalidate_dcache() {
	dcache_civac_all();
}

inline void clean_dcache() {
	dcache_civac_all();
}

template<typename ptr>
inline void invalidate_dcache_by_addr(ptr addr) {
	invalidate_dcache_address(reinterpret_cast<uintptr_t>(addr));
	dsb_sy();
}

template<typename ptr>
inline void invalidate_dcache_by_addr_fast(ptr addr) {
	invalidate_dcache_address(reinterpret_cast<uintptr_t>(addr));
}

template<typename ptr>
inline void clean_dcache_by_addr(ptr addr) {
	clean_dcache_address(reinterpret_cast<uintptr_t>(addr));
	dsb_sy();
}

template<typename ptr>
inline void clean_dcache_by_addr_fast(ptr addr) {
	clean_dcache_address(reinterpret_cast<uintptr_t>(addr));
}

inline void invalidate_dcache_by_range(void *addr, uint32_t size) {
	auto start_addr = reinterpret_cast<uintptr_t>(addr) & CacheLineMask;
	auto end_addr = (reinterpret_cast<uintptr_t>(addr) + size + CacheLineBytes - 1) & CacheLineMask;

	for (uintptr_t a = start_addr; a < end_addr; a += CacheLineBytes) {
		invalidate_dcache_address(a);
	}
	dsb_sy();
}

inline void clean_dcache_by_range(void *addr, uint32_t size) {
	auto start_addr = reinterpret_cast<uintptr_t>(addr) & CacheLineMask;
	auto end_addr = (reinterpret_cast<uintptr_t>(addr) + size + CacheLineBytes - 1) & CacheLineMask;

	for (uintptr_t a = start_addr; a < end_addr; a += CacheLineBytes) {
		clean_dcache_address(a);
	}
	dsb_sy();
}

inline void clean_and_invalidate_dcache_by_range(void *addr, int32_t size) {
	auto start_addr = reinterpret_cast<uintptr_t>(addr) & CacheLineMask;
	auto end_addr = (reinterpret_cast<uintptr_t>(addr) + size + CacheLineBytes - 1) & CacheLineMask;

	for (uintptr_t a = start_addr; a < end_addr; a += CacheLineBytes) {
		clean_invalidate_dcache_address(a);
	}
	dsb_sy();
}

} // namespace mdrivlib::SystemCache
