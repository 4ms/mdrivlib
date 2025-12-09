#pragma once
#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "stm32mp1xx.h"
#include "stm32mp1xx_hal_dac.h"
#include <algorithm>

namespace mdrivlib
{

struct DacConfig {
	int dac_num;
	PinDef dac0_pin;
	PinDef dac1_pin;

	DAC_ChannelConfTypeDef chan0;
	DAC_ChannelConfTypeDef chan1;
};

// This is a two-channel DAC
struct DualDac {
	DualDac(const DacConfig conf) {
		hdac.Instance = DAC1; // only one dac on this chip
		RCC_Enable::DAC12_::set();

		HAL_DAC_Init(&hdac);

		Pin init0{conf.dac0_pin, PinMode::Analog};
		//const_cast because STM32 HAL is not const-correct
		HAL_DAC_ConfigChannel(&hdac, const_cast<DAC_ChannelConfTypeDef *>(&conf.chan0), DAC_CHANNEL_1);
		HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
		set(0, 0);

		Pin init1{conf.dac1_pin, PinMode::Analog};
		HAL_DAC_ConfigChannel(&hdac, const_cast<DAC_ChannelConfTypeDef *>(&conf.chan1), DAC_CHANNEL_2);
		HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
		set(1, 0);
	}

	void set(uint32_t channel, uint16_t value) {
		value = std::clamp<uint32_t>(value, 0, 4095);
		if (channel == 0)
			DAC1->DHR12R1 = value;
		else
			DAC1->DHR12R2 = value;
	}

private:
	DAC_HandleTypeDef hdac;
};

} // namespace mdrivlib
