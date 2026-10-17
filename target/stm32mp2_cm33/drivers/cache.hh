#pragma once
#include <cstdint>

// Same API as the A35's cache.hh so shared drivers compile unchanged.
// No-ops for now: the M33's cache (ICACHE/DCACHE peripherals) is not enabled yet.
// TODO: implement with the DCACHE peripheral when the M33 runs from DDR with caching on.

namespace mdrivlib::SystemCache
{

inline void mem_barrier() {
	__asm volatile("dmb" ::: "memory");
}

inline void invalidate_dcache() {
}

template<typename ptr>
inline void invalidate_dcache_by_addr(ptr) {
}

inline void invalidate_dcache_by_range(void *, uint32_t) {
}

inline void clean_dcache() {
}

template<typename ptr>
inline void clean_dcache_by_addr(ptr) {
}

inline void clean_dcache_by_range(void *, uint32_t) {
}

inline void clean_and_invalidate_dcache_by_range(void *, int32_t) {
}
} // namespace mdrivlib::SystemCache
