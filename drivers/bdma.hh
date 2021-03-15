#pragma once
#include "arch.hh"
#include "debug.hh" //FixMe: remove when done debugging (not an mdrivlib file!)
#include "dma_config_struct.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

template<typename ConfT>
struct BDMATransfer {
	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	BDMA_Channel_TypeDef *stream;
	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_te_flag_index;

	DMA_HandleTypeDef hdma_spi6_tx;

	BDMATransfer() {
		target::RCC_Control::BDMA_P::set();

		if constexpr (ConfT::StreamNum == 0)
			stream = BDMA_Channel0;
		if constexpr (ConfT::StreamNum == 1)
			stream = BDMA_Channel1;
		if constexpr (ConfT::StreamNum == 2)
			stream = BDMA_Channel2;
		if constexpr (ConfT::StreamNum == 3)
			stream = BDMA_Channel3;
		if constexpr (ConfT::StreamNum == 4)
			stream = BDMA_Channel4;
		if constexpr (ConfT::StreamNum == 5)
			stream = BDMA_Channel5;
		if constexpr (ConfT::StreamNum == 6)
			stream = BDMA_Channel6;
		if constexpr (ConfT::StreamNum == 7)
			stream = BDMA_Channel7;

		dma_tc_flag_index = dma_get_TC_flag_index(stream);
		dma_te_flag_index = dma_get_TE_flag_index(stream);
		dma_isr_reg = dma_get_ISR_reg(stream);
		dma_ifcr_reg = dma_get_IFCR_reg(stream);
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		HAL_NVIC_DisableIRQ(ConfT::IRQn);
		InterruptManager::registerISR(ConfT::IRQn, ConfT::pri, ConfT::subpri, [=]() {
			if (*dma_isr_reg & dma_tc_flag_index) {
				*dma_ifcr_reg = dma_tc_flag_index;
				callback();
			}
			if (*dma_isr_reg & dma_te_flag_index) {
				*dma_ifcr_reg = dma_te_flag_index;
				// Error: debug breakpoint or logging here
			}
		});
	}

	void config_transfer(uint32_t dst, uint32_t src, size_t sz) {
		_transfer_size = sz;
		_src_addr = src;
		_dst_addr = dst;

		// Todo: use the registers Luke!
		hdma_spi6_tx.Instance = stream;
		hdma_spi6_tx.Init.Request = ConfT::RequestNum;
		hdma_spi6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_spi6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi6_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_spi6_tx.Init.Mode = DMA_NORMAL;
		hdma_spi6_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_spi6_tx) != HAL_OK) {
			__BKPT();
		}
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		config_transfer(reinterpret_cast<uint32_t>(dst), reinterpret_cast<uint32_t>(src), sz);
	}

	// void repeat_transfer_new_dst(void *dst) {
	// }

	void start_transfer() {
		//Some register needs to be set here to do a repeated xfer
		HAL_NVIC_EnableIRQ(ConfT::BDMAConf::IRQn);
		HAL_DMA_Start_IT(&hdma_spi6_tx, _src_addr, _dst_addr, _transfer_size);
	}

	uint32_t get_transfer_size() {
		return _transfer_size;
	}

private:
	void _set_addrs() {
		// MDMA0::SrcAddr::write(_src_addr);
		// MDMA0::DstAddr::write(_dst_addr);
	}

	// struct MDMA0 {
	// 	static constexpr uint32_t CBNDTR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CBNDTR);
	// 	using BlockNumDataBytesToXfer = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BNDT_Pos, 17>;
	// 	using BlockRepeatSrcUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRSUM_Pos, 1>;
	// 	using BlockRepeatDstUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRDUM_Pos, 1>;
	// 	using BlockRepeatCount = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRC_Pos, 12>;

	// 	static constexpr uint32_t CCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CCR);
	// 	using Enable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_EN_Pos, 1>;
	// 	using SWRequest = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_SWRQ_Pos, 1>;
	// 	using BufferTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TCIE_Pos, 1>;
	// 	using BlockTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BTIE_Pos, 1>;
	// 	using BlockRepeatTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BRTIE_Pos, 1>;
	// 	using ChannelTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_CTCIE_Pos, 1>;
	// 	using TransferErrISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TEIE_Pos, 1>;

	// 	static constexpr uint32_t CIFCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CIFCR);
	// 	using BufferTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CLTCIF_Pos, 1>;
	// 	using BlockTransferComplISRClear = RegisterSection<ReadWrite, CCR_Base, MDMA_CIFCR_CBTIF_Pos, 1>;
	// 	using BlockRepeatTransferComplISRClear = RegisterSection<ReadWrite, CCR_Base, MDMA_CIFCR_CBRTIF_Pos, 1>;
	// 	using ChannelTransferComplISRClear = RegisterSection<ReadWrite, CCR_Base, MDMA_CIFCR_CCTCIF_Pos, 1>;
	// 	using TransferErrISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CTEIF_Pos, 1>;

	// 	static constexpr uint32_t CTCR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTCR);
	// 	using BufferableWriteMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_BWM_Pos, 1>;
	// 	using SWRequestMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SWRM_Pos, 1>;
	// 	using TriggerMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_TRGM_Pos, 2>;
	// 	using PaddingAlignmentMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_PAM_Pos, 2>;
	// 	using PackEnable = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_PKE_Pos, 1>;
	// 	using TransferLength = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_TLEN_Pos, 7>;
	// 	using DstBurst = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DBURST_Pos, 3>;
	// 	using SrcBurst = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SBURST_Pos, 3>;

	// 	enum Sizes { Byte = 0b00, HalfWord = 0b01, Word = 0b10, DoubleWord = 0b11 };
	// 	using DstIncAmount = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DINCOS_Pos, 2>;
	// 	using SrcIncAmount = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SINCOS_Pos, 2>;
	// 	using DstSize = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DSIZE_Pos, 2>;
	// 	using SrcSize = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SSIZE_Pos, 2>;

	// 	enum Directions { DoNotInc = 0b00, Up = 0b10, Down = 0b11 };
	// 	using DstIncDir = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DINC_Pos, 2>;
	// 	using SrcIncDir = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SINC_Pos, 2>;

	// 	using DstAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CDAR), 0xFFFFFFFF>;
	// 	using SrcAddr = RegisterBits<ReadWrite, MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CSAR), 0xFFFFFFFF>;

	// 	static constexpr uint32_t CTBR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CTBR);
	// 	using DstBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_DBUS_Pos, 1>;
	// 	using SrcBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_SBUS_Pos, 1>;

	// 	static constexpr uint32_t CISR_Base = MDMA_Channel0_BASE + offsetof(MDMA_Channel_TypeDef, CISR);
	// 	using RequestIsActive = RegisterSection<ReadOnly, CISR_Base, MDMA_CISR_CRQA_Pos, 1>;
	// };
};
