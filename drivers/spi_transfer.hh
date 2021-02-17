#pragma once
#include "interrupt.hh"
#include "spi.hh"
#include "system.hh"
#include "util/math.hh"

template<typename ConfT>
struct SpiTransferDriver {
	SpiTransferDriver()
		: _ready(true) {

		spi.disable();
		spi.configure();

		spi.set_tx_message_size(1);
		spi.enable();
	}

private:
	SpiPeriph<typename ConfT::SpiConf> spi;
	typename ConfT::AuxPin dcpin;

	volatile bool _ready;
	void wait_until_ready() {
		while (!_ready) {
		}
		_ready = false;
	}

protected:
	enum PacketType { Cmd, Data };

	void enable_ISR() {
		InterruptManager::registerISR(ConfT::SpiConf::IRQn, [this]() {
			if (spi.is_end_of_transfer()) {
				spi.clear_EOT_flag();
				spi.clear_TXTF_flag();
				_ready = true;
			}
		});
		auto pri = System::encode_nvic_priority(ConfT::SpiConf::priority1, ConfT::SpiConf::priority2);
		NVIC_SetPriority(ConfT::SpiConf::IRQn, pri);
		NVIC_EnableIRQ(ConfT::SpiConf::IRQn);
		spi.enable_end_of_xfer_interrupt();
	}

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
		spi.load_tx_data(halfword);
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
	void transmit_open_data32(uint32_t word) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(word);
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

