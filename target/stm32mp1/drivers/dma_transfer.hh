#pragma once
#include "debug.hh" //REMOVE ME
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
	DMA_Stream_TypeDef *stream;
	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_ht_flag_index;
	uint32_t dma_te_flag_index;
	uint32_t dma_fe_flag_index;

	DMAMUX_Channel_TypeDef *dmamux_chan;

	// ST-HAL:
	DMA_HandleTypeDef hdma_tx;

	DMATransfer() {
		RCC_Enable::DMAMUX_::set();

		if constexpr (ConfT::DMAx == 1) {
			RCC_Enable::DMA1_::set();

			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA1_Stream0;
				dmamux_chan = DMAMUX1_Channel0;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA1_Stream1;
				dmamux_chan = DMAMUX1_Channel1;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA1_Stream2;
				dmamux_chan = DMAMUX1_Channel2;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA1_Stream3;
				dmamux_chan = DMAMUX1_Channel3;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA1_Stream4;
				dmamux_chan = DMAMUX1_Channel4;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA1_Stream5;
				dmamux_chan = DMAMUX1_Channel5;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA1_Stream6;
				dmamux_chan = DMAMUX1_Channel6;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA1_Stream7;
				dmamux_chan = DMAMUX1_Channel7;
			}
		}

		if constexpr (ConfT::DMAx == 2) {
			RCC_Enable::DMA2_::set();

			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA2_Stream0;
				dmamux_chan = DMAMUX1_Channel0;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA2_Stream1;
				dmamux_chan = DMAMUX1_Channel1;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA2_Stream2;
				dmamux_chan = DMAMUX1_Channel2;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA2_Stream3;
				dmamux_chan = DMAMUX1_Channel3;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA2_Stream4;
				dmamux_chan = DMAMUX1_Channel4;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA2_Stream5;
				dmamux_chan = DMAMUX1_Channel5;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA2_Stream6;
				dmamux_chan = DMAMUX1_Channel6;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA2_Stream7;
				dmamux_chan = DMAMUX1_Channel7;
			}
		}
		dma_tc_flag_index = dma_get_TC_flag_index(stream);
		dma_te_flag_index = dma_get_TE_flag_index(stream);
		dma_fe_flag_index = dma_get_FE_flag_index(stream);
		dma_ht_flag_index = dma_get_HT_flag_index(stream);
		dma_isr_reg = dma_get_ISR_reg(stream);
		dma_ifcr_reg = dma_get_IFCR_reg(stream);

		init();
	}

	void init() {
		// Todo: allow more detailed Configuration in ConfT:
		// FIFO enable and level
		// Data alignment: byte, halfword, word
		// Direction: M->P or P->M or M->M
		hdma_tx.Init.Request = ConfT::RequestNum;
		hdma_tx.Instance = stream;
		hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_tx.Init.Mode = DMA_NORMAL;
		hdma_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
		hdma_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		hdma_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
		hdma_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;

		HAL_DMA_DeInit(&hdma_tx);
		HAL_DMA_Init(&hdma_tx);
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
				__BKPT();
				*dma_ifcr_reg = dma_te_flag_index;
			}
			if (*dma_isr_reg & dma_fe_flag_index) {
				__BKPT();
				*dma_ifcr_reg = dma_fe_flag_index;
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
		hdma_tx.Lock = HAL_UNLOCKED;
		hdma_tx.State = HAL_DMA_STATE_READY;
		HAL_DMA_Start_IT(&hdma_tx, _src_addr, _dst_addr, _transfer_size / 2);
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

private:
	void init_DMAMUX() {
	}

	void init_DMAMUX_request() {
	}
};
} // namespace mdrivlib
