#pragma once
#include "stm32xx.h"
#include "util/math.hh"

namespace mdrivlib
{
namespace stm32h7x5
{
struct MPU_ {
	static void disable() {
		// Make sure outstanding transfers are done
		__DMB();
		// Disable fault exceptions
		SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
		// Disable the MPU and clear the control register
		MPU->CTRL = 0;
	}

	static void enable() {
		MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
		SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
		__DSB();
		__ISB();
	}

private:
	static inline uint32_t _region_ctr = 0;
	static const uint32_t INSTR_ACCESS_ENABLE = 0;
	static const uint32_t REGION_FULL_ACCESS = 3;
	static const uint32_t ACCESS_SHAREABLE = 1;
	static const uint32_t ACCESS_NOT_CACHEABLE = 0;
	static const uint32_t ACCESS_NOT_BUFFERABLE = 0;

public:
	// Returns region_id
	template<typename T>
	static uint32_t disable_cache_for_dma_buffer(T *baseaddr, uint32_t region_size) {
		auto region_id = _disable_cache_for_dma_buffer(reinterpret_cast<uint32_t>(baseaddr), region_size, _region_ctr);
		_region_ctr++;
		return region_id;
	}

private:
	static uint32_t _disable_cache_for_dma_buffer(uint32_t baseaddr, uint32_t region_size, uint32_t region_id) {
		if (_region_ctr > 15)
			return -1; // ERROR! too many MPU regions defined

		disable();

		uint32_t log2_region_size = MathTools::log2_floor(region_size);
		if (MathTools::is_power_of_2(region_size))
			log2_region_size--;	

		MPU->RNR = region_id;
		MPU->RBAR = baseaddr;

		MPU->RASR = (INSTR_ACCESS_ENABLE << MPU_RASR_XN_Pos) | (REGION_FULL_ACCESS << MPU_RASR_AP_Pos) |
					(0x00 << MPU_RASR_TEX_Pos) | (ACCESS_SHAREABLE << MPU_RASR_S_Pos) |
					(ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos) | (ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos) |
					(0x00 << MPU_RASR_SRD_Pos) | (log2_region_size << MPU_RASR_SIZE_Pos) |
					(0x01 << MPU_RASR_ENABLE_Pos);
		enable();

		return _region_ctr;
	}
};
} // namespace stm32h7x5
} // namespace mdrivlib
