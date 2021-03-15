#pragma once
#include "arch.hh"
#include "debug.hh" //FixMe: remove when done debugging (not an mdrivlib file!)
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

// Todo: template on channel#
struct MemoryTransfer {
	MDMA_HandleTypeDef hmdma;
	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	template<typename CallbackT>
	void registerCallback(CallbackT &&callback) {
		InterruptManager::registerISR(MDMA_IRQn, 2, 2, [=]() {
			if ((MDMA_Channel0->CISR & MDMA_CISR_BRTIF) && (MDMA_Channel0->CCR & MDMA_CCR_BRTIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CBRTIF;
			}

			if ((MDMA_Channel0->CISR & MDMA_CISR_BTIF) && (MDMA_Channel0->CCR & MDMA_CCR_BTIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CBTIF;
			}

			if ((MDMA_Channel0->CISR & MDMA_CISR_CTCIF) && (MDMA_Channel0->CCR & MDMA_CCR_CTCIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CCTCIF;
				// M2P large buffers (64kB):
				Debug::Pin1::low();
			}

			// Todo: check if size>127bytes (multi-buffer block), the right ISR is set
			if ((MDMA_Channel0->CISR & MDMA_CISR_TCIF) && (MDMA_Channel0->CCR & MDMA_CCR_TCIE)) {
				MDMA_Channel0->CIFCR = MDMA_CIFCR_CLTCIF;
				// M2M small buffers:
				// callback();
				Debug::Pin2::high();
				callback();
				Debug::Pin2::low();
			}
		});
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		_transfer_size = sz;
		_src_addr = reinterpret_cast<uint32_t>(src);
		_dst_addr = reinterpret_cast<uint32_t>(dst);

		target::RCC_Control::MDMA_::set();

		// Todo: use the registers Luke!
		hmdma.Instance = MDMA_Channel0;
		hmdma.Init.Request = MDMA_REQUEST_SW;
		hmdma.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER; // MDMA_BUFFER_TRANSFER;
		hmdma.Init.Priority = MDMA_PRIORITY_LOW;
		hmdma.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
		hmdma.Init.SourceInc = MDMA_SRC_INC_BYTE;			// WORD;
		hmdma.Init.DestinationInc = MDMA_DEST_INC_DISABLE;	// MDMA_DEST_INC_WORD;
		hmdma.Init.SourceDataSize = MDMA_SRC_DATASIZE_BYTE; // WORD
		hmdma.Init.DestDataSize = MDMA_DEST_DATASIZE_BYTE;	// WORD;
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
		// auto err = HAL_MDMA_Init(&hmdma);
		// if (err != HAL_OK)
		// 	__BKPT();

		// With WORD for Src
		// CTCR: D3FC 0222
		// SINC b10
		// DINC 0
		// SSIZE 0b10
		// DSIZE 0
		// SINCOS 0b10
		// DINCOS 0
		// SBUSRT= DBURST = 0b000
		// TLEN = 0b111 1111
		// PKE = 1
		// PAM = 0b00
		// TRGM = 0b01
		// BWM = 1

		// Try BWM = 0: when 0, transfer is ~3ms, screen shows more filled
		// Try TransferLength = 64 (or less to match SPI FIFO size), 32:same
		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(_transfer_size);
		MDMA0::BlockRepeatCount::write(0);

		MDMA0::SrcSize::write(MDMA0::Byte);
		MDMA0::SrcIncDir::write(MDMA0::Up);
		MDMA0::SrcIncAmount::write(MDMA0::Byte);
		MDMA0::DstSize::write(MDMA0::Byte);
		MDMA0::DstIncDir::write(MDMA0::DoNotInc);
		MDMA0::DstIncAmount::write(MDMA0::Byte);
		MDMA0::TransferLength::write(sz >= 128 ? 127 : sz - 1);
		// MDMA0::TransferLength::write(7);
		if (sz > 127)
			MDMA0::TriggerMode::write(0b01); // Block transfer
		else
			MDMA0::TriggerMode::write(0b00); // Buffer transfer
		MDMA0::PaddingAlignmentMode::write(0b00);
		MDMA0::PackEnable::set();
		MDMA0::BufferableWriteMode::clear();

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
		MDMA0::ChannelTransferComplISREnable::set();
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

		static constexpr uint32_t CTCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTCR);
		using BufferableWriteMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_BWM_Pos, 1>;
		using SWRequestMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SWRM_Pos, 1>;
		using TriggerMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_TRGM_Pos, 2>;
		using PaddingAlignmentMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_PAM_Pos, 2>;
		using PackEnable = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_PKE_Pos, 1>;
		using TransferLength = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_TLEN_Pos, 7>;
		using DstBurst = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DBURST_Pos, 3>;
		using SrcBurst = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SBURST_Pos, 3>;

		enum Sizes { Byte = 0b00, HalfWord = 0b01, Word = 0b10, DoubleWord = 0b11 };
		using DstIncAmount = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DINCOS_Pos, 2>;
		using SrcIncAmount = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SINCOS_Pos, 2>;
		using DstSize = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DSIZE_Pos, 2>;
		using SrcSize = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SSIZE_Pos, 2>;

		enum Directions { DoNotInc = 0b00, Up = 0b10, Down = 0b11 };
		using DstIncDir = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DINC_Pos, 2>;
		using SrcIncDir = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SINC_Pos, 2>;

		using DstAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CDAR), 0xFFFFFFFF>;
		using SrcAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CSAR), 0xFFFFFFFF>;

		static constexpr uint32_t CTBR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTBR);
		using DstBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_DBUS_Pos, 1>;
		using SrcBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_SBUS_Pos, 1>;

		static constexpr uint32_t CISR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CISR);
		using RequestIsActive = RegisterSection<ReadOnly, CISR_Base, MDMA_CISR_CRQA_Pos, 1>;
	};
};
