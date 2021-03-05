#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

struct MemoryTransfer {
	MDMA_HandleTypeDef hmdma;

	template<typename CallbackT>
	void registerCallback(CallbackT callback) {
		InterruptManager::registerISR(MDMA_IRQn, 1, 1, [&]() {
			HAL_MDMA_IRQHandler(&hmdma);
			callback();
		});
	}

	void start(void *dst, const void *src, size_t sz) {
		target::RCC_Control::MDMA_::set();

		hmdma.Instance = MDMA_Channel0;
		hmdma.Init.Request = MDMA_REQUEST_SW;
		hmdma.Init.TransferTriggerMode = MDMA_BUFFER_TRANSFER;
		hmdma.Init.Priority = MDMA_PRIORITY_MEDIUM;
		hmdma.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
		hmdma.Init.SourceInc = MDMA_SRC_INC_WORD;
		hmdma.Init.DestinationInc = MDMA_DEST_INC_WORD;
		hmdma.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
		hmdma.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
		hmdma.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
		hmdma.Init.BufferTransferLength = sz;
		hmdma.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
		hmdma.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
		hmdma.Init.SourceBlockAddressOffset = 0;
		hmdma.Init.DestBlockAddressOffset = 0;
		auto err = HAL_MDMA_Init(&hmdma);
		if (err != HAL_OK)
			__BKPT();

		err = HAL_MDMA_Start_IT(&hmdma, reinterpret_cast<uint32_t>(src), reinterpret_cast<uint32_t>(dst), sz, 1);
		if (err != HAL_OK)
			__BKPT();
	}
};
