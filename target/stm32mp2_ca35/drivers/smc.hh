#pragma once
// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
#include <cstdint>

namespace mdrivlib
{

struct smccc_res {
	uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
};

inline smccc_res
smc_call(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
	smccc_res r;
	register uint64_t x0 asm("x0") = a0;
	register uint64_t x1 asm("x1") = a1;
	register uint64_t x2 asm("x2") = a2;
	register uint64_t x3 asm("x3") = a3;
	register uint64_t x4 asm("x4") = a4;
	register uint64_t x5 asm("x5") = a5;
	register uint64_t x6 asm("x6") = a6;
	register uint64_t x7 asm("x7") = a7;

	asm volatile("smc #0"
				 : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3), "+r"(x4), "+r"(x5), "+r"(x6), "+r"(x7)
				 :
				 : "memory");
	r.a0 = x0;
	r.a1 = x1;
	r.a2 = x2;
	r.a3 = x3;
	r.a4 = x4;
	r.a5 = x5;
	r.a6 = x6;
	r.a7 = x7;

	return r;
}

} // namespace mdrivlib
