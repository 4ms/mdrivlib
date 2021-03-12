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

	DmaSpiScreenDriver(BDMA_Config &conf) {
		target::RCC_Control::BDMA_P::set();

		dma_tc_flag_index = dma_get_TC_flag_index(conf.stream);
		dma_te_flag_index = dma_get_TE_flag_index(conf.stream);
		dma_isr_reg = dma_get_ISR_reg(conf.stream);
		dma_ifcr_reg = dma_get_IFCR_reg(conf.stream);

		InterruptManager::registerISR(conf.IRQn, conf.pri, conf.subpri, [&]() {
			if (*dma_isr_reg & dma_tc_flag_index) {
				*dma_ifcr_reg = dma_tc_flag_index;
			}
			if (*dma_isr_reg & dma_te_flag_index) {
				*dma_ifcr_reg = dma_te_flag_index;
				// Error: debug breakpoint or logging here
			}
		});
		// HAL_NVIC_EnableIRQ(conf.IRQn);
	}

	void init() {
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;

	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_te_flag_index;
};
