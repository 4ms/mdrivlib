#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <cstdint>
#include <functional>

struct SMPControl {
	static inline __attribute__((section(".noncachable"))) uint32_t regs[4] = {0, 0, 0, 0};

	template<uint32_t channel>
	static void notify() {
		if constexpr (channel == 1)
			SecondaryCore::send_sgi(SGI1_IRQn);
		else if (channel == 2)
			SecondaryCore::send_sgi(SGI2_IRQn);
		else if (channel == 3)
			SecondaryCore::send_sgi(SGI3_IRQn);
		else if (channel == 4)
			SecondaryCore::send_sgi(SGI4_IRQn);
	}

	static void notify(uint32_t channel) {
		SecondaryCore::send_sgi(static_cast<IRQn_Type>(channel));
	}

	template<uint32_t reg_num = 0>
	static void write(uint32_t value) {
		if constexpr (reg_num == 0)
			regs[0] = value;
		else if (reg_num == 1)
			regs[1] = value;
		else if (reg_num == 2)
			regs[2] = value;
		else if (reg_num == 3)
			regs[3] = value;
	}

	static void write(uint32_t reg_num, uint32_t value) {
		if (reg_num >= 4)
			return;
		regs[reg_num] = value;
	}

	template<uint32_t reg_num = 0>
	static uint32_t read() {
		if constexpr (reg_num == 0)
			return regs[0];
		else if (reg_num == 1)
			return regs[1];
		else if (reg_num == 2)
			return regs[2];
		else if (reg_num == 3)
			return regs[3];
		return 0;
	}

	static uint32_t read(uint32_t reg_num) {
		return reg_num < 4 ? regs[reg_num] : 0;
	}
};

// Todo: keep this class synced with the aux_core implementation
// Assumes thread calls SMPControl::write<0>(0) when done;
// SGI3 is used for "call function"
struct SMPThread {
	enum { DataReg = 0 };

	static inline std::function<void()> thread_func;

	// Runs a function on the secondary core.
	static void run(std::function<void()> &&entry) {
		thread_func = std::move(entry);
		auto thread_func_addr = reinterpret_cast<uint32_t>(&thread_func);
		SMPControl::write<DataReg>(thread_func_addr);
		SMPControl::notify<3>();
	}

	template<uint32_t command_id, uint32_t data_reg = DataReg>
	static void run_command(uint32_t data) {
		SMPControl::write<data_reg>(data);
		SMPControl::notify<command_id>();
	}

	// Waits until thread completes
	static void join() {
		while (SMPControl::read<DataReg>() != 0)
			;
	}

	// Returns true if thread is completed
	static bool is_running() {
		return (SMPControl::read<DataReg>() != 0);
	}

	static void init() {
		// SMPControl::write<DataReg>(0);
	}
};
