#pragma once
#include "drivers/dma_config_struct.hh"
#include "drivers/dma_registers.hh"
#include "drivers/interrupt.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"
#include "stm32mp13xx_ll_etzpc.h"

// #define PRINT_ERRORS_DMA_TRANSFER

#ifdef PRINT_ERRORS_DMA_TRANSFER
#define pr_dma_err printf
#include <cstdio>
#else
#define pr_dma_err(...)
#endif

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

	bool fifo_error = false;
	bool transfer_error = false;

	// DMAMUX_Channel_TypeDef *dmamux_chan;

	// ST-HAL:
	DMA_HandleTypeDef hdma;

	DMATransfer() {
		if (!RCC_Enable::DMAMUX_::read())
			RCC_Enable::DMAMUX_::set();

		if constexpr (ConfT::DMAx == 1) {
			if (!RCC_Enable::DMA1_::read())
				RCC_Enable::DMA1_::set();

			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA1_Stream0;
				// dmamux_chan = DMAMUX1_Channel0;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA1_Stream1;
				// dmamux_chan = DMAMUX1_Channel1;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA1_Stream2;
				// dmamux_chan = DMAMUX1_Channel2;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA1_Stream3;
				// dmamux_chan = DMAMUX1_Channel3;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA1_Stream4;
				// dmamux_chan = DMAMUX1_Channel4;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA1_Stream5;
				// dmamux_chan = DMAMUX1_Channel5;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA1_Stream6;
				// dmamux_chan = DMAMUX1_Channel6;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA1_Stream7;
				// dmamux_chan = DMAMUX1_Channel7;
			}
		}

		if constexpr (ConfT::DMAx == 2) {
			if (!RCC_Enable::DMA2_::read())
				RCC_Enable::DMA2_::set();

			//See RM sec 19.3.2:
			// • DMAMUX channels 0 to 7 are connected to DMA1 channels 0 to 7
			// • DMAMUX channels 8 to 15 are connected to DMA2 channels 0 to 7
			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA2_Stream0;
				// dmamux_chan = DMAMUX1_Channel8;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA2_Stream1;
				// dmamux_chan = DMAMUX1_Channel9;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA2_Stream2;
				// dmamux_chan = DMAMUX1_Channel10;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA2_Stream3;
				// dmamux_chan = DMAMUX1_Channel11;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA2_Stream4;
				// dmamux_chan = DMAMUX1_Channel12;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA2_Stream5;
				// dmamux_chan = DMAMUX1_Channel13;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA2_Stream6;
				// dmamux_chan = DMAMUX1_Channel14;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA2_Stream7;
				// dmamux_chan = DMAMUX1_Channel15;
			}
		}
		if constexpr (ConfT::DMAx == 3) {
			if (!RCC_Enable::DMA3_::read())
				RCC_Enable::DMA3_::set();

			if (!RCC_Enable::DMAMUX_SECURE::read())
				RCC_Enable::DMAMUX_SECURE::set();

			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA3_Stream0;
				// dmamux_chan = DMAMUX2_Channel0;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA3_Stream1;
				// dmamux_chan = DMAMUX2_Channel1;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA3_Stream2;
				// dmamux_chan = DMAMUX2_Channel2;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA3_Stream3;
				// dmamux_chan = DMAMUX2_Channel3;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA3_Stream4;
				// dmamux_chan = DMAMUX2_Channel4;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA3_Stream5;
				// dmamux_chan = DMAMUX2_Channel5;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA3_Stream6;
				// dmamux_chan = DMAMUX2_Channel6;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA3_Stream7;
				// dmamux_chan = DMAMUX2_Channel7;
			}
		}

		if constexpr (ConfT::DMAx != 0) {
			dma_tc_flag_index = dma_get_TC_flag_index(stream);
			dma_te_flag_index = dma_get_TE_flag_index(stream);
			dma_fe_flag_index = dma_get_FE_flag_index(stream);
			dma_ht_flag_index = dma_get_HT_flag_index(stream);
			dma_isr_reg = dma_get_ISR_reg(stream);
			dma_ifcr_reg = dma_get_IFCR_reg(stream);

			init();
		}
	}

	void init() {

		hdma.Instance = stream;

		hdma.Init.Request = ConfT::RequestNum;

		hdma.Init.Direction = ConfT::dir == DefaultDMAConf::Mem2Periph ? DMA_MEMORY_TO_PERIPH
							: ConfT::dir == DefaultDMAConf::Periph2Mem ? DMA_PERIPH_TO_MEMORY
																	   : DMA_MEMORY_TO_MEMORY;
		hdma.Init.PeriphInc = DMA_PINC_DISABLE;

		hdma.Init.MemInc = DMA_MINC_ENABLE;

		hdma.Init.PeriphDataAlignment = ConfT::transfer_size_periph == DefaultDMAConf::Byte ? DMA_PDATAALIGN_BYTE
									  : ConfT::transfer_size_periph == DefaultDMAConf::HalfWord
										  ? DMA_PDATAALIGN_HALFWORD
										  : DMA_PDATAALIGN_WORD;

		hdma.Init.MemDataAlignment = ConfT::transfer_size_mem == DefaultDMAConf::Byte	  ? DMA_MDATAALIGN_BYTE
								   : ConfT::transfer_size_mem == DefaultDMAConf::HalfWord ? DMA_MDATAALIGN_HALFWORD
																						  : DMA_MDATAALIGN_WORD;
		hdma.Init.Mode = ConfT::circular ? DMA_CIRCULAR : ConfT::periph_flow ? DMA_PFCTRL : DMA_NORMAL;

		hdma.Init.Priority = ConfT::dma_priority == DefaultDMAConf::Low	   ? DMA_PRIORITY_LOW
						   : ConfT::dma_priority == DefaultDMAConf::Medium ? DMA_PRIORITY_MEDIUM
						   : ConfT::dma_priority == DefaultDMAConf::High   ? DMA_PRIORITY_HIGH
																		   : DMA_PRIORITY_VERY_HIGH;
		hdma.Init.FIFOMode = ConfT::enable_fifo ? DMA_FIFOMODE_ENABLE : DMA_FIFOMODE_DISABLE;

		hdma.Init.FIFOThreshold =
			ConfT::fifo_threshold == DefaultDMAConf::Fifo1QuarterFull	? DMA_FIFO_THRESHOLD_1QUARTERFULL
			: ConfT::fifo_threshold == DefaultDMAConf::FifoHalfFull		? DMA_FIFO_THRESHOLD_HALFFULL
			: ConfT::fifo_threshold == DefaultDMAConf::Fifo3QuarterFull ? DMA_FIFO_THRESHOLD_3QUARTERSFULL
																		: DMA_FIFO_THRESHOLD_FULL;

		hdma.Init.MemBurst = ConfT::mem_burst == DefaultDMAConf::Single ? DMA_MBURST_SINGLE
						   : ConfT::mem_burst == DefaultDMAConf::Inc4	? DMA_MBURST_INC4
						   : ConfT::mem_burst == DefaultDMAConf::Inc8	? DMA_MBURST_INC8
																		: DMA_MBURST_INC16;

		hdma.Init.PeriphBurst = ConfT::mem_burst == DefaultDMAConf::Single ? DMA_PBURST_SINGLE
							  : ConfT::mem_burst == DefaultDMAConf::Inc4   ? DMA_PBURST_INC4
							  : ConfT::mem_burst == DefaultDMAConf::Inc8   ? DMA_PBURST_INC8
																		   : DMA_PBURST_INC16;
		auto res = HAL_DMA_DeInit(&hdma);
		if (res != HAL_OK)
			pr_dma_err("HAL DMA DeInit error %d\n", res);

		res = HAL_DMA_Init(&hdma);
		if (res != HAL_OK)
			pr_dma_err("HAL DMA Init error %d\n", res);
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
				// __BKPT(32);
				*dma_ifcr_reg = dma_te_flag_index;
				transfer_error = true;
			}
			if (*dma_isr_reg & dma_fe_flag_index) {
				// __BKPT(33);
				*dma_ifcr_reg = dma_fe_flag_index;
				fifo_error = true;
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

	bool had_transfer_error() {
		auto err = transfer_error;
		transfer_error = false;
		return err;
	}

	bool had_fifo_error() {
		auto err = fifo_error;
		fifo_error = false;
		return err;
	}

	// constrain: param has a .DMA_Handle of type DMA_HandleTypeDef*
	void link_periph_to_dma(auto &periph_hal_handle) {
		periph_hal_handle.DMA_Handle = &hdma;
		hdma.Parent = &periph_hal_handle;
	}

private:
	void init_DMAMUX() {
	}

	void init_DMAMUX_request() {
	}
};
} // namespace mdrivlib
