#pragma once
#include "interrupt.hh"
#include "spi.hh"
#include "spi_transfer_config_struct.hh"
#include "system.hh"
#include "util/math.hh"

// Arch-specific wrapper around a chip's SPI peripheral, handling multi-chip buses.
// Tx-only..  but may be extended for full duplex or rx-only

// init()
// send_blocking()

//???
// register_transfer_complete_ISR(func) //or just do it ourselves
// send_with_ISR(uint32_t size, DataPacketT *data)
// register_dma_xfer_complete_ISR(func) //or just do it ourselves
// send_with_DMA(uint32_t size, DataPacketT *data)

// These allow for container of this to flip aux gpio pins etc: (data/cmd, dac latch, etc):

// transmit_blocking

template<typename SpiConfT>
struct SpiTransferDriver {

	SpiTransferDriver() {
		unselect_all_chips();
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(0);
		spi.enable();
	}

public: /*private:*/
	SpiPeriph<SpiConfT> spi;
	uint32_t _cur_chip = 0;

public:
	// // is this needed?
	// void enable_EOT_ISR() {
	// 	InterruptManager::registerISR(SpiConfT::IRQn, [this]() {
	// 		if (spi.is_end_of_transfer()) {
	// 			spi.clear_EOT_flag();
	// 			spi.clear_TXTF_flag();
	// 			_ready = true;
	// 		}
	// 	});
	// 	_ready = true;
	// 	auto pri = System::encode_nvic_priority(SpiConfT::priority1, SpiConfT::priority2);
	// 	NVIC_SetPriority(SpiConfT::IRQn, pri);
	// 	NVIC_EnableIRQ(SpiConfT::IRQn);
	// 	spi.enable_end_of_xfer_interrupt();
	// }

	void init() {
		spi.disable();
		spi.clear_TXTF_flag(); // needed?
		spi.enable();
	}

	void transmit_blocking(uint32_t word) {
		wait_until_ready_to_tx();
		// spi.disable();
		// spi.set_fifo_threshold(1);
		// spi.enable();
		spi.load_tx_data(word);
		spi.start_transfer();
		wait_until_tx_complete();
	}

	void begin_open_transmission() {
		spi.start_transfer();
	}

	// Remove this?
	/*
	void begin_open_data_transmission(uint8_t fifo_size) {
		wait_until_ready();
		spi.disable();
		spi.set_fifo_threshold(fifo_size);
		spi.clear_TXTF_flag(); // needed?
		spi.enable();
		spi.start_transfer();
	}*/

	void transmit_when_ready(uint32_t word) {
		wait_until_ready_to_tx();
		spi.load_tx_data(word);
	}
	void transmit(uint32_t word) {
		spi.load_tx_data(word);
	}

	bool is_tx_complete() {
		return spi.is_tx_complete();
	}
	void wait_until_tx_complete() {
		while (!spi.is_tx_complete())
			;
	}
	bool is_ready_to_tx() {
		return spi.tx_space_available();
	}
	void wait_until_ready_to_tx() {
		while (!spi.tx_space_available())
			;
	}

	void end_open_data_transmission() {
		// needed?
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.enable();
	}

	uint32_t cur_chip() const {
		return _cur_chip;
	}
	void select_chip(uint32_t chip) {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.select(chip);
	}
	void unselect_chip(uint32_t chip) {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.unselect(chip);
	}
	template<uint32_t chip>
	void select_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.template select<chip>();
	}
	template<uint32_t chip>
	void unselect_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.template unselect<chip>();
	}

	void select_cur_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.select(_cur_chip);
	}
	void unselect_cur_chip() {
		if constexpr (SpiConfT::use_hardware_ss == true)
			return;
		if constexpr (SpiConfT::use_hardware_ss == false)
			spi.unselect(_cur_chip);
	}

	void unselect_all_chips() {
		auto tmp = _cur_chip;
		do {
			spi.unselect(_cur_chip);
			advance_chip();
		} while (_cur_chip != tmp);
	}

	void advance_chip() {
		if constexpr (SpiConfT::NumChips == 1)
			return;
		if constexpr (SpiConfT::NumChips == 2) {
			_cur_chip = 1 - _cur_chip;
			return;
		}
		if constexpr (SpiConfT::NumChips > 2) {
			if (++_cur_chip >= SpiConfT::NumChips)
				_cur_chip = 0;
		}
	}
};

