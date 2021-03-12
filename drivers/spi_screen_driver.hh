#pragma once
#include "dma_config_struct.hh"
#include "interrupt.hh"
#include "spi.hh"
#include "system.hh"
#include "util/math.hh"

// Todo: this is not necessarily an SPI screen driver, it's a general SPI driver that has a hi/low pin for each transfer
// But we can extend this to utilize DMA:
// transmit writes into a framebuffer
// timer ISR executes refresh
// Look into DMA2D with TouchGFX

template<typename ConfT>
struct SpiScreenDriver {
	SpiScreenDriver()
		: _ready(true) {
		InterruptManager::registerISR(
			ConfT::ScreenSpiConf::IRQn, ConfT::ScreenSpiConf::priority1, ConfT::ScreenSpiConf::priority2, [this]() {
				if (spi.is_end_of_transfer()) {
					spi.clear_EOT_flag();
					spi.clear_TXTF_flag();
					_ready = true;
				}
			});

		spi.disable();
		spi.configure();
		NVIC_EnableIRQ(ConfT::ScreenSpiConf::IRQn);

		spi.set_tx_message_size(1);
		spi.enable();
		spi.enable_end_of_xfer_interrupt();
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;
	volatile bool _ready;
	// uint32_t _data_ctr;
	void wait_until_ready() {
		while (!_ready) {
		}
		_ready = false;
	}

protected:
	enum PacketType { Cmd, Data };

	template<PacketType MessageType>
	void transmit(uint8_t byte) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(1);
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();
		spi.load_tx_data(byte);
		spi.start_transfer();
	}

	void transmit_data_16(uint16_t halfword) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(2);
		spi.enable();
		dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes16(halfword));
		spi.start_transfer();
	}

	void transmit_data_32(uint16_t halfword1, uint16_t halfword2) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(4);
		spi.enable();
		dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes_and_combine(halfword2, halfword1));
		spi.start_transfer();
	}

	void transmit_data_32(uint32_t word) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(4);
		spi.enable();
		dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes32(word));
		spi.start_transfer();
	}

	void begin_open_data_transmission(uint8_t fifo_size) {
		wait_until_ready();
		while (!spi.tx_space_available())
			;
		spi.disable();
		spi.set_tx_message_size(0);
		spi.set_fifo_threshold(fifo_size);
		spi.disable_end_of_xfer_interrupt();
		spi.clear_TXTF_flag();
		spi.enable();
		dcpin.high();
		spi.start_transfer();
	}

	void transmit_open_data16(uint16_t halfword) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(halfword);
	}

	void transmit_open_data32(uint16_t halfword1, uint16_t halfword2) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(MathTools::swap_bytes_and_combine(halfword2, halfword1));
	}

	void transmit_open_data32(uint32_t word) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(MathTools::swap_bytes32(word));
	}

	void end_open_data_transmission() {
		spi.disable();
		spi.set_tx_message_size(1);
		spi.set_fifo_threshold(1);
		spi.enable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		_ready = true;
		spi.enable_end_of_xfer_interrupt();
	}
};

template<typename ConfT>
struct DmaSpiScreenDriver {

	DmaSpiScreenDriver() {
		target::RCC_Control::BDMA_P::set();
	}

	void init_dma() {
		BDMA_Channel_TypeDef *stream;
		if constexpr (ConfT::BDMAConf::StreamNum == 2)
			stream = BDMA_Channel2;

		hdma_spi6_tx.Instance = stream;
		hdma_spi6_tx.Init.Request = ConfT::BDMAConf::RequestNum;
		hdma_spi6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_spi6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi6_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_spi6_tx.Init.Mode = DMA_NORMAL;
		hdma_spi6_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_spi6_tx) != HAL_OK) {
			__BKPT();
		}

		// #define __HAL_LINKDMA(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
//                         do{                                                      \
//                               (__HANDLE__)->__PPP_DMA_FIELD__ = &(__DMA_HANDLE__); \
//                               (__DMA_HANDLE__).Parent = (__HANDLE__);

		dma_tc_flag_index = dma_get_TC_flag_index(stream);
		dma_te_flag_index = dma_get_TE_flag_index(stream);
		dma_isr_reg = dma_get_ISR_reg(stream);
		dma_ifcr_reg = dma_get_IFCR_reg(stream);

		InterruptManager::registerISR(ConfT::BDMAConf::IRQn, ConfT::BDMAConf::pri, ConfT::BDMAConf::subpri, [&]() {
			if (*dma_isr_reg & dma_tc_flag_index) {
				*dma_ifcr_reg = dma_tc_flag_index;
			}
			if (*dma_isr_reg & dma_te_flag_index) {
				*dma_ifcr_reg = dma_te_flag_index;
				// Error: debug breakpoint or logging here
			}
		});
		HAL_NVIC_DisableIRQ(ConfT::BDMAConf::IRQn);
	}

	void init() {
		spi.configure();
		init_dma();
		spi.enable();
	}

	void start_dma_transfer(uint32_t src, uint32_t sz) {
		spi.enable_tx_dma();
		// if (sz <= 0xFFFF) {
		// 	spi.set_tx_message_size(sz);
		// 	spi.set_tx_message_reload_size(0);
		// } else if (sz <= (0xFFFF * 2)) {
		// 	spi.set_tx_message_size(0xFFFF);
		// 	spi.set_tx_message_reload_size(sz - 0xFFFF);
		// } else
		// 	return; // Todo: handle transfers > 128kB by setting up an ISR on TSERF that sets the reload size again
		uint32_t dst = spi.get_tx_datareg_addr();
		HAL_DMA_Start(&hdma_spi6_tx, src, dst, sz);
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;

	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_te_flag_index;

	DMA_HandleTypeDef hdma_spi6_tx;
};
