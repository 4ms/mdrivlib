#pragma once
#include "arch.hh"
#include "debug.hh" //FixMe: remove when done debugging (not an mdrivlib file!)
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

// ConfT:
static constexpr unsigned channel = 0;
static constexpr bool swap_bytes = true;
static constexpr bool swap_halfwords = false;
static constexpr bool swap_words = false;

// Todo: template on channel#
// And template on ConfigType:
// which ISRs are set
// which ISR gets the callback (or is it always CTC?)
// src/dst inc, dir, amount
//...
struct MemoryTransfer {
	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	MemoryTransfer() {
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		InterruptManager::registerISR(MDMA_IRQn, 0, 2, [=]() {
			// if (MDMA0::BlockRepeatTransferComplISRFlag::read() && MDMA0::BlockRepeatTransferComplISREnable::read()) {
			// 	MDMA0::BlockRepeatTransferComplISRClear::set();
			// }

			// if (MDMA0::BlockTransferComplISRFlag::read() && MDMA0::BlockTransferComplISREnable::read()) {
			// 	MDMA0::BlockTransferComplISRClear::set();
			// }

			if (MDMA0::ChannelTransferComplISRFlag::read() && MDMA0::ChannelTransferComplISREnable::read()) {
				MDMA0::ChannelTransferComplISRClear::set();
				callback();
			}

			// if (MDMA0::BufferTransferComplISRFlag::read() && MDMA0::BufferTransferComplISREnable::read()) {
			// 	MDMA0::BufferTransferComplISRClear::set();
			// }

			if (MDMA0::TransferErrISRFlag::read() && MDMA0::TransferErrISREnable::read()) {
				MDMA0::TransferErrISRClear::set();
			}
		});
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		_transfer_size = sz;
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_src_addr = reinterpret_cast<uint32_t>(src);

		target::RCC_Control::MDMA_::set();
		MDMA0::Enable::clear();
		MDMA0::BlockNumDataBytesToXfer::write(_transfer_size);
		MDMA0::BlockRepeatCount::write(0);

		MDMA0::SrcSize::write(MDMA0::Word);
		MDMA0::SrcIncDir::write(MDMA0::Up);
		MDMA0::SrcIncAmount::write(MDMA0::Word);
		MDMA0::DstSize::write(MDMA0::Word);
		MDMA0::DstIncDir::write(MDMA0::Up);
		MDMA0::DstIncAmount::write(MDMA0::Word);
		MDMA0::TransferLength::write(sz >= 128 ? 127 : sz - 1);
		if (sz > 127)
			MDMA0::TriggerMode::write(0b01); // Block transfer
		else
			MDMA0::TriggerMode::write(0b00); // Buffer transfer
		MDMA0::PaddingAlignmentMode::write(0b00);
		MDMA0::PackEnable::set();
		MDMA0::BufferableWriteMode::clear();

		if constexpr (swap_bytes)
			MDMA0::ByteEndianessExchange::set();
		if constexpr (swap_halfwords)
			MDMA0::HalfwordEndianessExchange::set();
		if constexpr (swap_words)
			MDMA0::WordEndianessExchange::set();



		MDMA0::BufferTransferComplISRClear::set();
		MDMA0::BlockTransferComplISRClear::set();
		MDMA0::BlockRepeatTransferComplISRClear::set();
		MDMA0::ChannelTransferComplISRClear::set();
		MDMA0::TransferErrISRClear::set();

		_set_addrs();
		_set_addr_bus_bits();

		MDMA0::TransferErrISREnable::set();
		// MDMA0::BufferTransferComplISREnable::set();
		// MDMA0::BlockTransferComplISREnable::set();
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
		static constexpr uint32_t CISR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CISR);
		using RequestIsActive = RegisterSection<ReadOnly, CISR_Base, MDMA_CISR_CRQA_Pos, 1>;
		using BufferTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_TCIF_Pos, 1>;
		using BlockTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_BTIF_Pos, 1>;
		using BlockRepeatTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_BRTIF_Pos, 1>;
		using ChannelTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_CTCIF_Pos, 1>;
		using TransferErrISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_TEIF_Pos, 1>;

		static constexpr uint32_t CIFCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CIFCR);
		using BufferTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CLTCIF_Pos, 1>;
		using BlockTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CBTIF_Pos, 1>;
		using BlockRepeatTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CBRTIF_Pos, 1>;
		using ChannelTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CCTCIF_Pos, 1>;
		using TransferErrISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CTEIF_Pos, 1>;

		static constexpr uint32_t CESR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CESR);
		// Todo

		static constexpr uint32_t CCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CCR);
		using SWRequest = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_SWRQ_Pos, 1>;
		using WordEndianessExchange = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_WEX_Pos, 1>;
		using HalfwordEndianessExchange = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_HEX_Pos, 1>;
		using ByteEndianessExchange = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BEX_Pos, 1>;
		using PriorityLevel = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_PL_Pos, 1>;
		using BufferTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TCIE_Pos, 1>;
		using BlockTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BTIE_Pos, 1>;
		using BlockRepeatTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BRTIE_Pos, 1>;
		using ChannelTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_CTCIE_Pos, 1>;
		using TransferErrISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TEIE_Pos, 1>;
		using Enable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_EN_Pos, 1>;

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

		static constexpr uint32_t CBNDTR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CBNDTR);
		using BlockRepeatCount = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRC_Pos, 12>;
		using BlockRepeatDstUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRDUM_Pos, 1>;
		using BlockRepeatSrcUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRSUM_Pos, 1>;
		using BlockNumDataBytesToXfer = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BNDT_Pos, 17>;

		static constexpr uint32_t CTBR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTBR);
		using DstBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_DBUS_Pos, 1>;
		using SrcBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_SBUS_Pos, 1>;
		using TriggerSelection = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_TSEL_Pos, 6>;

		using DstAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CDAR), 0xFFFFFFFF>;
		using SrcAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CSAR), 0xFFFFFFFF>;

		// Todo: CBRUR
		// Todo: CLAR
		// Todo: CMAR
		// Todo: CMDR
	};
};
