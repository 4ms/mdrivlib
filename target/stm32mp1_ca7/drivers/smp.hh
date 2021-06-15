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
	// Runs a function on the secondary core.
	// Function must be owned by caller and lifetime must last until function is over, at least
	static void run(std::function<void()> &entry) {
		auto thread_func_addr = reinterpret_cast<uint32_t>(&entry);
		SMPControl::write(thread_func_addr);
		SMPControl::notify<3>();
	}

	static void join() {
		while (SMPControl::read() != 0)
			;
	}
};
