#pragma once
#include "stm32xx.h"

struct BDMA_Config {
	BDMA_TypeDef *BDMAx;
	BDMA_Channel_TypeDef *stream;
	uint32_t channel;
	IRQn_Type IRQn;
	uint32_t pri;
	uint32_t subpri;
	// bool continuous;
};

struct BDMA_Conf {
	static constexpr unsigned BDMAx = 0;
	static constexpr unsigned StreamNum = 0;
	static constexpr unsigned RequestNum = 0;
	static constexpr IRQn_Type IRQn = HardFault_IRQn;
	static constexpr uint32_t pri = 0;
	static constexpr uint32_t subpri = 0;

	enum Direction { Mem2Mem, Mem2Periph, Periph2Mem, Periph2Periph };
	static constexpr Direction dir = Mem2Periph;

	static constexpr bool circular = false;

	enum TransferSize {Byte, HalfWord, Word};
	static constexpr TransferSize transfer_size_mem = Byte; //Dest, in P2P
	static constexpr TransferSize transfer_size_periph = Byte; //Source, in M2M

	// Todo: Double-buffer mode

	enum Priority { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };
	static constexpr uint8_t dma_priority = Low;

	static constexpr bool mem_inc = true;
	static constexpr bool periph_inc = false;

	static constexpr bool half_transfer_interrupt_enable = false;
};

