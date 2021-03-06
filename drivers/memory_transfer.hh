#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

struct MemoryTransfer {
	MDMA_HandleTypeDef hmdma;
	uint32_t src_addr;
	uint32_t dst_addr;
	uint32_t xfer_size;

	// template<typename CallbackT>
	// void registerCallback(CallbackT &&callback) {
	// 	InterruptManager::registerISR(MDMA_IRQn, 1, 1, [=]() {
	// 		if ((MDMA_Channel0->CISR & MDMA_CISR_BRTIF) && (MDMA_Channel0->CCR & MDMA_CCR_BRTIE)) {
	// 			MDMA_Channel0->CIFCR = MDMA_CIFCR_CBRTIF;
	// 		}

	// 		if ((MDMA_Channel0->CISR & MDMA_CISR_BTIF) && (MDMA_Channel0->CCR & MDMA_CCR_BTIE)) {
	// 			MDMA_Channel0->CIFCR = MDMA_CIFCR_CBTIF;
	// 		}

	// 		if ((MDMA_Channel0->CISR & MDMA_CISR_CTCIF) && (MDMA_Channel0->CCR & MDMA_CCR_CTCIE)) {
	// 			MDMA_Channel0->CIFCR = MDMA_CIFCR_CCTCIF;
	// 			callback();
	// 		}

	// 		if ((MDMA_Channel0->CISR & MDMA_CISR_TCIF) && (MDMA_Channel0->CCR & MDMA_CCR_TCIE)) {
	// 			MDMA_Channel0->CIFCR = MDMA_CIFCR_CLTCIF;
	// 			// callback();
	// 		}
	// 	});
	// }

	struct MDMA0 {
		static constexpr uint32_t CBNDTR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CBNDTR);
		using BlockNumDataBytesToXfer = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BNDT_Pos, 17>;
		using BlockRepeatSrcUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRSUM_Pos, 1>;
		using BlockRepeatDstUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRDUM_Pos, 1>;
		using BlockRepeatCount = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRC_Pos, 12>;

		static constexpr uint32_t CCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CCR);
		using Enable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_EN_Pos, 1>;
		using SWRequest = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_SWRQ_Pos, 1>;

		using DstAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CDAR), 0xFFFFFFFF>;
		using SrcAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CSAR), 0xFFFFFFFF>;

		static constexpr uint32_t CTBR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTBR);
		using DstBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_DBUS_Pos, 1>;
		using SrcBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_SBUS_Pos, 1>;

		static constexpr uint32_t CISR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CISR);
		using RequestIsActive = RegisterSection<ReadOnly, CISR_Base, MDMA_CISR_CRQA_Pos, 1>;
	};

	void first_start(void *dst, const void *src, size_t sz) {
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
		hmdma.XferBufferCpltCallback = NULL;
		hmdma.XferBlockCpltCallback = NULL;
		hmdma.XferRepeatBlockCpltCallback = NULL;
		hmdma.XferCpltCallback = NULL;
		hmdma.XferAbortCallback = NULL;
		hmdma.XferErrorCallback = NULL;
		auto err = HAL_MDMA_Init(&hmdma);
		if (err != HAL_OK)
			__BKPT();

		xfer_size = sz;
		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(xfer_size);
		MDMA0::BlockRepeatCount::write(0);
		MDMA_Channel0->CIFCR =
			MDMA_CIFCR_CBRTIF | MDMA_CIFCR_CTEIF | MDMA_CIFCR_CCTCIF | MDMA_CIFCR_CLTCIF | MDMA_CIFCR_CBTIF;

		src_addr = reinterpret_cast<uint32_t>(src);
		dst_addr = reinterpret_cast<uint32_t>(dst);
		_set_addrs();
		_set_addr_bus_bits();

		MDMA_Channel0->CCR =
			MDMA_Channel0->CCR | MDMA_CCR_TEIE | MDMA_CCR_TCIE | MDMA_CCR_BTIE; // | MDMA_CCR_BRTIE | MDMA_CCR_CTCIE;

		repeat_start();
	}

	void repeat_start() {
		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(xfer_size);
		MDMA0::BlockRepeatCount::write(0);
		_set_addrs();
		MDMA0::Enable::set();
		// Debug point:
		if (MDMA0::RequestIsActive::read())
			while (1)
				;
		MDMA0::SWRequest::set();
	}

	void _set_addrs() {
		MDMA0::SrcAddr::write(src_addr);
		MDMA0::DstAddr::write(dst_addr);
	}

	void _set_addr_bus_bits() {
		uint32_t addressMask;
		addressMask = src_addr & 0xFF000000U;
		if ((addressMask == 0x20000000U) || (addressMask == 0x00000000U)) {
			MDMA0::SrcBusIsAHBTCM::set();
		} else {
			MDMA0::SrcBusIsAHBTCM::clear();
		}

		addressMask = dst_addr & 0xFF000000U;
		if ((addressMask == 0x20000000U) || (addressMask == 0x00000000U)) {
			MDMA0::DstBusIsAHBTCM::set();
		} else {
			MDMA0::DstBusIsAHBTCM::clear();
		}
		// const uint32_t src_bus = src_addr & 0xFF000000U;
		// MDMA0::SrcBusIsAHBTCM::write(src_bus == 0x20000000 || src_bus == 0);
		// const uint32_t dsr_bus = dst_addr & 0xFF000000U;
		// MDMA0::DstBusIsAHBTCM::write(dst_bus == 0x20000000 || dst_bus == 0);
	}
};
