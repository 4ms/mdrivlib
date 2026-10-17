// HAL timebase for the Cortex-M33 from the system timestamp generator (STGEN), the same
// free-running counter that drives the A35's generic timer (CNTPCT). So both cores share
// one timebase, and no SysTick interrupt is needed.
// The M33 reads the counter through STGENR (the read-only view), and the frequency from
// STGENC->CNTFID0, which TF-A sets (64MHz).
//
// Addresses: the M33 is built without CORTEX_IN_SECURE_STATE, so CMSIS gives the 0x4xxxxxxx
// aliases (same as the A35). It still runs secure (SAU off), so its accesses are secure.
// CMSIS has no plain STGENC name in that configuration, hence STGENC_NS.
#include "drivers/stm32xx.h"
#include <cstdint>

namespace
{
uint32_t cntfreq_kHz;
uint32_t cntfreq_MHz;

constexpr uint32_t DefaultCounterFreq = 64'000'000; // what TF-A sets

uint32_t counter_freq() {
	auto hz = STGENC_NS->CNTFID0;
	return hz ? hz : DefaultCounterFreq;
}

void read_counter_freq() {
	auto hz = counter_freq();
	cntfreq_kHz = hz / 1000;
	cntfreq_MHz = hz / 1000000;
}

uint32_t ticks_per_ms() {
	if (cntfreq_kHz == 0) [[unlikely]]
		read_counter_freq();
	return cntfreq_kHz;
}

uint32_t ticks_per_us() {
	if (cntfreq_MHz == 0) [[unlikely]]
		read_counter_freq();
	return cntfreq_MHz;
}
} // namespace

// 64-bit counter read from two 32-bit registers: re-read if the upper half changed
extern "C" uint64_t stgen_read_counter() {
	uint32_t hi, lo;
	do {
		hi = STGENR->CNTCVU;
		lo = STGENR->CNTCVL;
	} while (hi != STGENR->CNTCVU);
	return (uint64_t(hi) << 32) | lo;
}

extern "C" uint32_t stgen_counter_freq() {
	return counter_freq();
}

extern "C" HAL_StatusTypeDef HAL_InitTick(uint32_t) {
	read_counter_freq();
	return HAL_OK;
}

extern "C" uint32_t HAL_GetTick(void) {
	auto per_ms = ticks_per_ms();
	return per_ms ? uint32_t(stgen_read_counter() / per_ms) : 0;
}

extern "C" void udelay(unsigned int us) {
	auto start = stgen_read_counter();
	while ((stgen_read_counter() - start) < uint64_t(us) * ticks_per_us())
		;
}

extern "C" void mdelay(unsigned int ms) {
	auto start = stgen_read_counter();
	while ((stgen_read_counter() - start) < uint64_t(ms) * ticks_per_ms())
		;
}
