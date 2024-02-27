#pragma once
#include "drivers/dma_transfer.hh"
#include "drivers/interrupt.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"
#include <optional>

extern volatile bool sd_rx;
extern volatile bool sd_tx;

namespace mdrivlib
{

template<typename ConfT>
struct SDMMCTarget {

	struct DmaConf : mdrivlib::DefaultDMAConf {
		static constexpr auto DMAx = 2;
		static constexpr auto StreamNum = 3;
		static constexpr auto RequestNum = DMA_CHANNEL_4;
		static constexpr auto dma_priority = Low;
		static constexpr IRQn_Type IRQn = DMA2_Stream3_IRQn;
		static constexpr uint32_t pri = 0;
		static constexpr uint32_t subpri = 0;
	};

	static void setup_hal_handle(SD_HandleTypeDef &hsd) {
		auto has_clock_div = SDMMCTarget<ConfT>::calc_clock_div();

		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockBypass = has_clock_div.has_value() ? SDMMC_CLOCK_BYPASS_DISABLE : SDMMC_CLOCK_BYPASS_ENABLE;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = ConfT::width ? SDMMC_BUS_WIDE_4B : SDMMC_BUS_WIDE_1B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
		hsd.Init.ClockDiv = has_clock_div.value_or(0);
		//F7: source_clk = 48M, clock_div=0 => 48/(2+0) = 24MHz, clock_div=1 => 48/(2+1) = 16MHz

		// static DMATransfer<DmaConf> dma_;

		static DMA_HandleTypeDef dma_rx_handle;
		dma_rx_handle.Instance = DMA2_Stream3;
		dma_rx_handle.Init.Channel = DMA_CHANNEL_4;
		dma_rx_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
		dma_rx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_rx_handle.Init.MemInc = DMA_MINC_ENABLE;
		dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		dma_rx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		dma_rx_handle.Init.Mode = DMA_PFCTRL;
		dma_rx_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		dma_rx_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		dma_rx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		dma_rx_handle.Init.MemBurst = DMA_MBURST_INC4;
		dma_rx_handle.Init.PeriphBurst = DMA_PBURST_INC4;

		__HAL_LINKDMA(&hsd, hdmarx, dma_rx_handle);
		HAL_DMA_DeInit(&dma_rx_handle);
		HAL_DMA_Init(&dma_rx_handle);

		static DMA_HandleTypeDef dma_tx_handle;
		dma_tx_handle.Instance = DMA2_Stream6;
		dma_tx_handle.Init.Channel = DMA_CHANNEL_4;
		dma_tx_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
		dma_tx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_tx_handle.Init.MemInc = DMA_MINC_ENABLE;
		dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		dma_tx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		dma_tx_handle.Init.Mode = DMA_PFCTRL;
		dma_tx_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		dma_tx_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		dma_tx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		dma_tx_handle.Init.MemBurst = DMA_MBURST_INC4;
		dma_tx_handle.Init.PeriphBurst = DMA_PBURST_INC4;

		__HAL_LINKDMA(&hsd, hdmatx, dma_tx_handle);
		HAL_DMA_DeInit(&dma_tx_handle);
		HAL_DMA_Init(&dma_tx_handle);

		Interrupt::register_and_start_isr(SDMMC1_IRQn, 0, 0, [&] { HAL_SD_IRQHandler(&hsd); });
		Interrupt::register_and_start_isr(DMA2_Stream3_IRQn, 0, 0, [&] { HAL_DMA_IRQHandler(&dma_rx_handle); });
		Interrupt::register_and_start_isr(DMA2_Stream6_IRQn, 0, 0, [&] { HAL_DMA_IRQHandler(&dma_tx_handle); });
	}

	static bool read(SD_HandleTypeDef *hsd, uint8_t *data, uint32_t block_num, uint32_t numblocks) {
		sd_rx = false;
		if (HAL_SD_ReadBlocks_DMA(hsd, data, block_num, numblocks) != HAL_OK)
			return false;

		uint32_t timeout = HAL_GetTick();
		while (HAL_GetTick() - timeout < 1000) {
			if (sd_rx)
				return true;
		}
		return false;
	}

	static bool write(SD_HandleTypeDef *hsd, uint8_t *data, uint32_t block_num, uint32_t numblocks) {
		sd_tx = false;
		if (HAL_SD_WriteBlocks_DMA(hsd, data, block_num, numblocks) != HAL_OK)
			return false;

		uint32_t timeout = HAL_GetTick();
		while (HAL_GetTick() - timeout < 1000) {
			if (sd_tx)
				return true;
		}
		return false;
	}

	static void config_widebus(SD_HandleTypeDef *hsd) {
		//FIXME: Why does this fail?
		HAL_SD_ConfigWideBusOperation(hsd, SDMMC_BUS_WIDE_4B);
	}

private:
	static std::optional<uint32_t> calc_clock_div() {
		uint32_t source_clock;
		//Clock sel bit selects PLL48 clock or system clock
		constexpr uint32_t cksel_bit = RCC_DCKCFGR2_SDMMC1SEL;
		if ((RCC->DCKCFGR2 & cksel_bit) == 0)
			source_clock = 48'000'000;
		else
			source_clock = HAL_RCC_GetHCLKFreq(); //or HAL_RCC_GetSysClockFreq()

		if (ConfT::speed_hz == source_clock)
			return std::nullopt; //No clock divider

		// From RefManual: SDMMC_CK frequency = SDMMCCLK / [CLKDIV + 2]
		uint32_t clock_div_raw = source_clock / ConfT::speed_hz;
		if (clock_div_raw < 2)
			return 0;
		if (clock_div_raw > 257)
			return 255;
		return clock_div_raw - 2;
	}
};

} // namespace mdrivlib
