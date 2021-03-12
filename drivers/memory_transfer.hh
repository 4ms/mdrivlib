#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

struct MemoryTransfer {
	MDMA_HandleTypeDef hmdma;
	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	template<typename CallbackT>
	void registerCallback(CallbackT &&callback) {
		InterruptManager::registerISR(MDMA_IRQn, 0, 0, [=]() {
			if ((MDMA_Channel0->CISR & MDMA_CISR_BRTIF) && (MDMA_Channel0->CCR & MDMA_CCR_BRTIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CBRTIF;
			}

			if ((MDMA_Channel0->CISR & MDMA_CISR_BTIF) && (MDMA_Channel0->CCR & MDMA_CCR_BTIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CBTIF;
			}

			if ((MDMA_Channel0->CISR & MDMA_CISR_CTCIF) && (MDMA_Channel0->CCR & MDMA_CCR_CTCIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CCTCIF;
			}

			// Todo: check if size>127bytes (multi-buffer block), the right ISR is set
			if ((MDMA_Channel0->CISR & MDMA_CISR_TCIF) && (MDMA_Channel0->CCR & MDMA_CCR_TCIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CLTCIF;
				callback();
			}
		});
	}

	struct MDMA0 {
		static constexpr uint32_t CBNDTR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CBNDTR);
		using BlockNumDataBytesToXfer = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BNDT_Pos, 17>;
		using BlockRepeatSrcUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRSUM_Pos, 1>;
		using BlockRepeatDstUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRDUM_Pos, 1>;
		using BlockRepeatCount = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRC_Pos, 12>;

		static constexpr uint32_t CCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CCR);
		using Enable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_EN_Pos, 1>;
		using SWRequest = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_SWRQ_Pos, 1>;
		using BufferTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TCIE_Pos, 1>;
		using BlockTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BTIE_Pos, 1>;
		using BlockRepeatTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BRTIE_Pos, 1>;
		using ChannelTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_CTCIE_Pos, 1>;
		using TransferErrISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TEIE_Pos, 1>;

		static constexpr uint32_t CIFCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CIFCR);
		using BufferTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CLTCIF_Pos, 1>;
		using BlockTransferComplISRClear = RegisterSection<ReadWrite, CCR_Base, MDMA_CIFCR_CBTIF_Pos, 1>;
		using BlockRepeatTransferComplISRClear = RegisterSection<ReadWrite, CCR_Base, MDMA_CIFCR_CBRTIF_Pos, 1>;
		using ChannelTransferComplISRClear = RegisterSection<ReadWrite, CCR_Base, MDMA_CIFCR_CCTCIF_Pos, 1>;
		using TransferErrISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CTEIF_Pos, 1>;

		using DstAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CDAR), 0xFFFFFFFF>;
		using SrcAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CSAR), 0xFFFFFFFF>;

		static constexpr uint32_t CTBR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTBR);
		using DstBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_DBUS_Pos, 1>;
		using SrcBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_SBUS_Pos, 1>;

		static constexpr uint32_t CISR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CISR);
		using RequestIsActive = RegisterSection<ReadOnly, CISR_Base, MDMA_CISR_CRQA_Pos, 1>;
	};

	void config_transfer(void *dst, const void *src, size_t sz) {
		_transfer_size = sz;
		_src_addr = reinterpret_cast<uint32_t>(src);
		_dst_addr = reinterpret_cast<uint32_t>(dst);

		target::RCC_Control::MDMA_::set();

		// Todo: use the registers Luke!
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
		hmdma.Init.BufferTransferLength = _transfer_size;
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

		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(_transfer_size);
		MDMA0::BlockRepeatCount::write(0);
		MDMA0::BufferTransferComplISRClear::set();
		MDMA0::BlockTransferComplISRClear::set();
		MDMA0::BlockRepeatTransferComplISRClear::set();
		MDMA0::ChannelTransferComplISRClear::set();
		MDMA0::TransferErrISRClear::set();

		_set_addrs();
		_set_addr_bus_bits();

		MDMA0::TransferErrISREnable::set();
		MDMA0::BufferTransferComplISREnable::set();
		MDMA0::BlockTransferComplISREnable::set();
		// MDMA0::BlockRepeatTransferComplISREnable::set();
		// MDMA0::ChannelTransferComplISREnable::set();
	}

	void repeat_transfer_new_dst(void *dst) {
		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(_transfer_size);
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_set_addrs();
		MDMA0::Enable::set();
		MDMA0::SWRequest::set();
	}

	void start_transfer() {
		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(_transfer_size);
		_set_addrs();
		MDMA0::Enable::set();
		// Debug point:
		// if (MDMA0::RequestIsActive::read())
		// 	while (1)
		// 		;
		MDMA0::SWRequest::set();
	}

private:
	void _set_addrs() {
		MDMA0::SrcAddr::write(_src_addr);
		MDMA0::DstAddr::write(_dst_addr);
	}

	void _set_addr_bus_bits() {
		const uint32_t src_bus = _src_addr & 0xFF000000;
		MDMA0::SrcBusIsAHBTCM::write(src_bus == 0x20000000 || src_bus == 0);

		const uint32_t dst_bus = _dst_addr & 0xFF000000;
		MDMA0::DstBusIsAHBTCM::write(dst_bus == 0x20000000 || dst_bus == 0);
	}
};
