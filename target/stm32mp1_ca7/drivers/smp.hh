#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <cstdint>

struct SMPControl {
	static void notify(uint32_t channel) {
		if (channel == 1)
			SecondaryCore::send_sgi(SGI1_IRQn);
		if (channel == 2)
			SecondaryCore::send_sgi(SGI2_IRQn);
		if (channel == 3)
			SecondaryCore::send_sgi(SGI3_IRQn);
	}

	// static uint32_t read() {
	// 	return TAMP->BKP31R;
	// }

	// static void write(uint32_t value) {
	// 	TAMP->BKP31R = value;
	// }

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
