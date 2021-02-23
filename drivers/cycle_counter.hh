#pragma once

namespace mdrivlib
{

class CycleCounter {
public:
	CycleCounter() {
		init();
	}
	void init() {
		DWT->CTRL = DWT->CTRL | DWT_CTRL_CYCCNTENA_Msk;
	}

	void start_measurement() {
		_start_tm = read_cycle_count();
		_period = _start_tm - _last_start_tm;
		_last_start_tm = _start_tm;
	}

	void end_measurement() {
		uint32_t _measured_tm = read_cycle_count() - _start_tm;
	}

	uint32_t get_last_measurement_raw() {
		return _measured_tm;
	}

	uint32_t get_last_period_raw() {
		return _period;
	}

	float get_last_measurement_load_decimal() {
		if (_period == 0)
			return 0;
		return _measured_tm / _period;
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
	uint32_t _period = 1;

	uint32_t read_cycle_count() {
		return DWT->CYCCNT;
	}
};
} // namespace mdrivlib
