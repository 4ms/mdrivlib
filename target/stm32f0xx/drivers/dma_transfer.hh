#pragma once
#include "dma_config_struct.hh"
#include "dma_registers.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"

// Todo: Finish converting to using CMSIS instead of STM32-HAL
namespace mdrivlib
{
template<typename ConfT>
struct DMATransfer {
	using DMAX = DMA_<ConfT::DMAx, ConfT::StreamNum>;

	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	DMA_TypeDef *dmax;
	DMA_Channel_TypeDef *channel;
	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_ht_flag_index;
	uint32_t dma_te_flag_index;

	// ST-HAL:
	DMA_HandleTypeDef hdma;

	DMATransfer() {
		RCC_Enable::DMA1_::set();

		if constexpr (ConfT::StreamNum == 1) {
			channel = DMA1_Channel1;
		}
		if constexpr (ConfT::StreamNum == 2) {
			channel = DMA1_Channel2;
		}
		if constexpr (ConfT::StreamNum == 3) {
			channel = DMA1_Channel3;
		}
		if constexpr (ConfT::StreamNum == 4) {
			channel = DMA1_Channel4;
		}
		if constexpr (ConfT::StreamNum == 5) {
			channel = DMA1_Channel5;
		}

		dma_tc_flag_index = dma_get_TC_flag_index(channel);
		dma_te_flag_index = dma_get_TE_flag_index(channel);
		dma_ht_flag_index = dma_get_HT_flag_index(channel);
		dma_isr_reg = dma_get_ISR_reg(channel);
		dma_ifcr_reg = dma_get_IFCR_reg(channel);

		init();
	}

	void init() {
		hdma.Instance = channel;

		hdma.Init.Direction = ConfT::dir == DefaultDMAConf::Mem2Periph ? DMA_MEMORY_TO_PERIPH
							: ConfT::dir == DefaultDMAConf::Periph2Mem ? DMA_PERIPH_TO_MEMORY
																	   : DMA_MEMORY_TO_MEMORY;
		hdma.Init.PeriphInc = ConfT::periph_inc ? DMA_PINC_ENABLE : DMA_PINC_DISABLE;

		hdma.Init.MemInc = ConfT::mem_inc ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;

		hdma.Init.PeriphDataAlignment = ConfT::transfer_size_periph == DefaultDMAConf::Byte ? DMA_PDATAALIGN_BYTE
									  : ConfT::transfer_size_periph == DefaultDMAConf::HalfWord
										  ? DMA_PDATAALIGN_HALFWORD
										  : DMA_PDATAALIGN_WORD;

		hdma.Init.MemDataAlignment = ConfT::transfer_size_mem == DefaultDMAConf::Byte	  ? DMA_MDATAALIGN_BYTE
								   : ConfT::transfer_size_mem == DefaultDMAConf::HalfWord ? DMA_MDATAALIGN_HALFWORD
																						  : DMA_MDATAALIGN_WORD;
		hdma.Init.Mode = ConfT::circular ? DMA_CIRCULAR : DMA_NORMAL;

		hdma.Init.Priority = ConfT::dma_priority == DefaultDMAConf::Low	   ? DMA_PRIORITY_LOW
						   : ConfT::dma_priority == DefaultDMAConf::Medium ? DMA_PRIORITY_MEDIUM
						   : ConfT::dma_priority == DefaultDMAConf::High   ? DMA_PRIORITY_HIGH
																		   : DMA_PRIORITY_VERY_HIGH;
		HAL_DMA_DeInit(&hdma);
		HAL_DMA_Init(&hdma);
	}

	void register_callback(auto cb) {
		InterruptControl::disable_irq(ConfT::IRQn);
		InterruptManager::register_and_start_isr(ConfT::IRQn, ConfT::pri, ConfT::subpri, [callback = cb, this]() {
			if (*dma_isr_reg & dma_tc_flag_index) {
				*dma_ifcr_reg = dma_tc_flag_index;
				if constexpr (ConfT::half_transfer_interrupt_enable)
					callback(1);
				else
					callback();
			}
			if constexpr (ConfT::half_transfer_interrupt_enable) {
				if (*dma_isr_reg & dma_ht_flag_index) {
					*dma_ifcr_reg = dma_ht_flag_index;
					callback(0);
				}
			}
			if (*dma_isr_reg & dma_te_flag_index) {
				*dma_ifcr_reg = dma_te_flag_index;
			}
		});
	}

	void config_transfer(uint32_t dst, uint32_t src, size_t sz) {
		_transfer_size = sz;
		_src_addr = src;
		_dst_addr = dst;
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		config_transfer(reinterpret_cast<uint32_t>(dst), reinterpret_cast<uint32_t>(src), sz);
	}

	void start_transfer() {
		hdma.Lock = HAL_UNLOCKED;
		hdma.State = HAL_DMA_STATE_READY;
		HAL_DMA_Start_IT(&hdma, _src_addr, _dst_addr, _transfer_size / 2);
		// Todo: replace the HAL command with this:
		// DMAX::Enable::clear();
		// while (DMAX::Enable::read())
		// 	;
		// clear DMAMUX CFR synchro overrun
		// clear DMAMUX request gen overrun
		// DMAX::TransferComplClearFlag::set();
		// DMAX::HalfTransferComplClearFlag::set();
		// DMAX::TransferErrorClearFlag::set();
		// DMAX::DirectModeErrorClearFlag::set();
		// DMAX::FIFOErrorClearFlag::set();
		// clear DBM bit
		// DMAX::NDTR::write(_transfer_size/2)
		// DMAX::PAR::write(_dst_addr) //or src, if p to m
		// DMAX::M0AR::write(_src_addr) //or dst_addr, if p to m
		// enable interrupts: DMAX::TransferComplEnableFlag::set(); ... etc
		// DMAMUX sync overrun IT?
		// DMAMUX RequestGen IT?
		// DMAX::Enable::set();
	}

	uint32_t get_transfer_size() {
		return _transfer_size;
	}

	// constrain: param has a .DMA_Handle of type DMA_HandleTypeDef*
	void link_periph_to_dma(auto &periph_hal_handle) {
		periph_hal_handle.DMA_Handle = &hdma;
		hdma.Parent = &periph_hal_handle;
	}
};
} // namespace mdrivlib
