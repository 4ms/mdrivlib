#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <atomic>
#include <cstdint>
#include <functional>

namespace mdrivlib
{

struct SMPControl {
	static constexpr uint32_t NumCores = 2;
	static constexpr uint32_t NumRegs = 8;
	static inline __attribute__((section(".noncachable"))) std::atomic<uint32_t> regs[NumRegs] = {
		0, 0, 0, 0, 0, 0, 0, 0};

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
		if constexpr (reg_num < NumRegs)
			regs[reg_num].store(value);
	}

	static void write(uint32_t reg_num, uint32_t value) {
		if (reg_num >= NumRegs)
			return;
		regs[reg_num].store(value);
	}

	template<uint32_t reg_num = 0>
	static uint32_t read() {
		if constexpr (reg_num < NumRegs)
			return regs[reg_num].load();
		else
			return 0;
	}

	static uint32_t read(uint32_t reg_num) {
		return reg_num < NumRegs ? regs[reg_num].load() : 0;
	}
};

struct SMPThread {
	static constexpr uint32_t StatusReg = 0;
	enum Status { NotRunning = 0, Running = 1 };

	enum : uint32_t { CustomFunc0 = 0, CustomFunc1 = 0, CustomFunc2 = 2, CallFunction = 3 };

	static inline std::function<void()> thread_func;

	// Moves a function to static storage and notifies the secondary core that it should execute it
	// Aux Core must respond to the SGI#3 by calling SMPThread::execute();
	static void launch(std::function<void()> &&entry) {
		thread_func = std::move(entry);
		SMPControl::write<StatusReg>(Running);
		SMPControl::notify<CallFunction>();
	}

	// Called by the aux core to respond to receiving an SGI3
	static void execute() {
		thread_func();
		signal_done();
	}

	template<uint32_t command_id>
	static void split_with_command() {
		SMPControl::write<StatusReg>(Running);
		SMPControl::notify<command_id>();
	}

	// Waits until thread completes
	static void join() {
		while (SMPControl::read<StatusReg>() == Running)
			;
	}

	// Aux Core must call this after processing a custom command
	static void signal_done() {
		SMPControl::write<StatusReg>(NotRunning);
	}

	// Returns true if thread is completed
	static bool is_running() {
		return (SMPControl::read<StatusReg>() == Running);
	}

	static void init() {
		SMPControl::write<StatusReg>(NotRunning);
	}
};
} // namespace mdrivlib
