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
		else if (reg_num == 29)
			TAMP->BKP29R = value;
		else if (reg_num == 28)
			TAMP->BKP28R = value;
		else if (reg_num == 27)
			TAMP->BKP27R = value;
	}

	template<uint32_t reg_num = 31>
	static uint32_t read() {
		if constexpr (reg_num == 31)
			return TAMP->BKP31R;
		else if (reg_num == 30)
			return TAMP->BKP30R;
		else if (reg_num == 29)
			return TAMP->BKP29R;
		else if (reg_num == 28)
			return TAMP->BKP28R;
		else if (reg_num == 27)
			return TAMP->BKP27R;
	}
};

struct SMPThread {
	enum { NotRunning = 0, IsRunning = 1 };
	enum { StatusReg = 30, DataReg = 31 };
	static inline std::function<void()> thread_func;

	// Runs a function on the secondary core.
	static void run(std::function<void()> &&entry) {
		// while (is_running())
		// 	;
		thread_func = std::move(entry);
		auto thread_func_addr = reinterpret_cast<uint32_t>(&thread_func);
		SMPControl::write(thread_func_addr);
		SMPControl::write<StatusReg>(IsRunning);
		SMPControl::notify<3>();
	}

	static void join() {
		while (SMPControl::read() != 0)
			;
	}

	static bool is_running() {
		return (SMPControl::read<StatusReg>() == IsRunning);
	}

	static void init() {
		SMPControl::write<DataReg>(0);
		SMPControl::write<StatusReg>(NotRunning);
	}
};
