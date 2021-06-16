#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <cstdint>
#include <functional>

struct SMPControl {
	template<uint32_t channel>
	static void notify() {
		if constexpr (channel == 1)
			SecondaryCore::send_sgi(SGI1_IRQn);
		else if (channel == 2)
			SecondaryCore::send_sgi(SGI2_IRQn);
		else if (channel == 3)
			SecondaryCore::send_sgi(SGI3_IRQn);
	}

	template<uint32_t reg_num = 31>
	static void write(uint32_t value) {
		if constexpr (reg_num == 31)
			TAMP->BKP31R = value;
		else if (reg_num == 30)
			TAMP->BKP30R = value;
	}

	template<uint32_t reg_num = 31>
	static uint32_t read() {
		if constexpr (reg_num == 31)
			return TAMP->BKP31R;
		else if (reg_num == 30)
			return TAMP->BKP30R;
	}
};

struct SMPThread {
	enum { NotRunning = 0, IsRunning = 1 };
	enum { StatusReg = 30, DataReg = 31 };

	// Runs a function on the secondary core.
	// Function must be owned by caller and lifetime must last until function is over, at least
	static void run(std::function<void()> &entry) {
		lazy_init();

		while (is_running())
			;

		SMPControl::write<StatusReg>(IsRunning);
		auto thread_func_addr = reinterpret_cast<uint32_t>(&entry);
		SMPControl::write(thread_func_addr);
		SMPControl::notify<3>();
	}

	static void join() {
		while (SMPControl::read() != 0)
			;
	}

	static bool is_running() {
		return (SMPControl::read<StatusReg>() == IsRunning);
	}

private:
	static void lazy_init() {
		static bool _lazy_init = true;
		if (_lazy_init) {
			SMPControl::write<DataReg>(0);
			SMPControl::write<StatusReg>(NotRunning);
			_lazy_init = false;
		}
	}
};
