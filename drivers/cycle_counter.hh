#pragma once
#include "stm32xx.h"
#include <cstdint>

namespace mdrivlib
{
// namespace stm32h7x5 { //Todo: is this universal or target-specific?

class CycleCounter {
public:
	CycleCounter() {
		init();
	}

	void init() {
		if (!(DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk)) {
			CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
			DWT->LAR = 0xC5ACCE55;
			DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
		}
	}

	void start_measurement() {
		_start_tm = read_cycle_count();
		_period = _start_tm - _last_start_tm;
		_last_start_tm = _start_tm;
	}

	void end_measurement() {
		_measured_tm = read_cycle_count() - _start_tm;
	}

	uint32_t get_last_measurement_raw() {
		return _measured_tm;
	}

	uint32_t get_last_period_raw() {
		return _period;
	}

	float get_last_measurement_load_float() {
		if (_period == 0)
			return 0;
		return (float)_measured_tm / (float)_period;
	}

	uint32_t get_last_measurement_load_percent() {
		if (_period == 0)
			return 0;
		return (_measured_tm * 100) / _period;
	}

private:
	uint32_t _last_start_tm = 0;
	uint32_t _start_tm = 0;
	uint32_t _measured_tm = 0;
	uint32_t _period = 0;

	uint32_t read_cycle_count() {
		return DWT->CYCCNT;
	}
};
} // namespace mdrivlib
