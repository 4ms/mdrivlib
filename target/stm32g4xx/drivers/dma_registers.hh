#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct DMARegisters {

	static DMA_Channel_TypeDef *get_channel(DMA_TypeDef *DMAx, unsigned channum) {
		if (DMAx == DMA1) {
			if (channum == 1)
				return DMA1_Channel1;
			if (channum == 2)
				return DMA1_Channel2;
			if (channum == 3)
				return DMA1_Channel3;
			if (channum == 4)
				return DMA1_Channel4;
			if (channum == 5)
				return DMA1_Channel5;
			if (channum == 6)
				return DMA1_Channel6;
		}
		if (DMAx == DMA2) {
			if (channum == 1)
				return DMA2_Channel1;
			if (channum == 2)
				return DMA2_Channel2;
			if (channum == 3)
				return DMA2_Channel3;
			if (channum == 4)
				return DMA2_Channel4;
			if (channum == 5)
				return DMA2_Channel5;
			if (channum == 6)
				return DMA2_Channel6;
		}
		return nullptr;
	}
};

} // namespace mdrivlib
