#pragma once
#include "interrupt.hh"
#include "spi.hh"
#include "system.hh"

// Todo: this is not necessarily an SPI screen driver, it's a general SPI driver that has a hi/low pin for each transfer
// But we can extend this to utilize DMA:
// transmit writes into a framebuffer
// timer ISR executes refresh
// Look into DMA2D? TouchGFX?

template<typename ConfT>
struct SpiScreenDriver {
	SpiScreenDriver()
		: _ready(true) {
		InterruptManager::registerISR(ConfT::ScreenSpiConf::IRQn, [this]() {
			if (spi.is_end_of_transfer()) {
				spi.clear_EOT_flag();
				spi.clear_TXTF_flag();
				_ready = true;
			}
		});

		spi.configure();
		auto pri = System::encode_nvic_priority(ConfT::ScreenSpiConf::priority1, ConfT::ScreenSpiConf::priority2);
		NVIC_SetPriority(ConfT::ScreenSpiConf::IRQn, pri);
		NVIC_EnableIRQ(ConfT::ScreenSpiConf::IRQn);

		spi.set_tx_data_size(1);
		spi.enable();
		spi.enable_end_of_xfer_interrupt();
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;
	volatile bool _ready;

protected:
	enum PacketType { Cmd, Data };

	template<PacketType ByteType>
	void transmit(uint8_t byte) {
		while (!_ready) {
		}
		_ready = false;
		if constexpr (ByteType == Cmd)
			dcpin.low();
		if constexpr (ByteType == Data)
			dcpin.high();
		spi.load_tx_data(byte);
		spi.start_transfer();
	}
};

