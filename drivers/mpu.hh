#pragma once
// #include "core_cm7.h"
// #include "mpu_armv7.h"
// #include "stm32h7xx_hal_cortex.h"
#include "stm32xx.h"
#include "util/math.hh"

namespace mdrivlib
{
namespace stm32h7x5xx
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
		MPU->CTRL = MPU_PRIVILEGED_DEFAULT | MPU_CTRL_ENABLE_Msk;
		SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
		__DSB();
		__ISB();
	}

private:
	static inline uint32_t _region_ctr = 0;

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

		uint32_t region_size_log = MathTools::Log2Int(region_size) - 1;

		MPU->RNR = region_id;
		MPU->RBAR = baseaddr;

		const uint32_t instr_access_enable = 0;
		const uint32_t full_access = 3;
		MPU->RASR = (MPU_INSTRUCTION_ACCESS_ENABLE << MPU_RASR_XN_Pos) | (MPU_REGION_FULL_ACCESS << MPU_RASR_AP_Pos) |
					(0x00 << MPU_RASR_TEX_Pos) | (MPU_ACCESS_SHAREABLE << MPU_RASR_S_Pos) |
					(MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos) | (MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos) |
					(0x00 << MPU_RASR_SRD_Pos) | (region_size_log << MPU_RASR_SIZE_Pos) | (0x01 << MPU_RASR_ENABLE_Pos);

		// MPU_Region_InitTypeDef mpu_init;
		// mpu_init.Enable = MPU_REGION_ENABLE;
		// mpu_init.BaseAddress = baseaddr;
		// mpu_init.Size = region_size_log;
		// mpu_init.AccessPermission = MPU_REGION_FULL_ACCESS;
		// mpu_init.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
		// mpu_init.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
		// mpu_init.IsShareable = MPU_ACCESS_SHAREABLE;
		// mpu_init.Number = MPU_REGION_NUMBER0;
		// mpu_init.TypeExtField = MPU_TEX_LEVEL0;
		// mpu_init.SubRegionDisable = 0x00;
		// mpu_init.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
		// HAL_MPU_ConfigRegion(&mpu_init);

		enable();

		// if (region_id != (region_id & MPU_RBAR_REGION_Msk)) return; // error: bad region
		// // ARM_MPU_RBAR(region_id, baseaddr);
		return _region_ctr;
	}
};
} // namespace stm32h7x5xx
} // namespace mdrivlib
