#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <cstdint>
#include <functional>

struct SMPControl {
	static inline __attribute__((section(".sysram"))) uint32_t regs[4];

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
			regs[0] = value;
		else if (reg_num == 30)
			regs[1] = value;
		else if (reg_num == 29)
			regs[2] = value;
		else if (reg_num == 28)
			regs[3] = value;
	}

	template<uint32_t reg_num = 31>
	static uint32_t read() {
		if constexpr (reg_num == 31)
			return regs[0];
		else if (reg_num == 30)
			return regs[1];
		else if (reg_num == 29)
			return regs[2];
		else if (reg_num == 28)
			return regs[3];
	}
};

struct SMPThread {
	enum { NotRunning = 0, IsRunning = 1 };
	enum { StatusReg = 30, DataReg = 31 };
	static inline std::function<void()> thread_func;

	// Runs a function on the secondary core.
	static void run(std::function<void()> &&entry) {
		thread_func = std::move(entry);
		auto thread_func_addr = reinterpret_cast<uint32_t>(&thread_func);
		SMPControl::write<DataReg>(thread_func_addr);
		SMPControl::notify<3>();
	}

	static void join() {
		while (SMPControl::read<DataReg>() != 0)
			;
	}

	static bool is_running() {
		return (SMPControl::read<DataReg>() != 0);
	}

	static void init() {
		SMPControl::write<DataReg>(0);
	}
};
