#pragma once
#include "interrupt.hh"
#include "spi.hh"
#include "spi_transfer_config_struct.hh"
#include "system.hh"
#include "util/math.hh"

// Todo: make this an arch-specific wrapper around a chip's SPI peripheral
// Come up with a general interface:
// send_blocking()
// register_transfer_complete_ISR(func) //or just do it ourselves
// send_with_ISR(uint32_t size, DataPacketT *data)
// register_dma_xfer_complete_ISR(func) //or just do it ourselves
// send_with_DMA(uint32_t size, DataPacketT *data)

template<typename ConfT>
struct SpiTransferDriver {

	SpiTransferDriver() {
		unselect_all_chips();
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(0);
		spi.enable();
	}

private:
	SpiPeriph<typename ConfT::SpiConf> spi;
	uint32_t _cur_chan = 0;
	uint32_t _cur_chip = 0;

	void wait_until_ready() {
		while (!spi.tx_space_available())
			;
	}

public:
	// // is this needed?
	// void enable_EOT_ISR() {
	// 	InterruptManager::registerISR(ConfT::SpiConf::IRQn, [this]() {
	// 		if (spi.is_end_of_transfer()) {
	// 			spi.clear_EOT_flag();
	// 			spi.clear_TXTF_flag();
	// 			_ready = true;
	// 		}
	// 	});
	// 	_ready = true;
	// 	auto pri = System::encode_nvic_priority(ConfT::SpiConf::priority1, ConfT::SpiConf::priority2);
	// 	NVIC_SetPriority(ConfT::SpiConf::IRQn, pri);
	// 	NVIC_EnableIRQ(ConfT::SpiConf::IRQn);
	// 	spi.enable_end_of_xfer_interrupt();
	// }

	void init() {
		spi.enable();
	}
	void transmit_blocking(uint32_t word) {
		wait_until_ready();
		spi.disable();
		spi.set_fifo_threshold(1);
		spi.enable();
		spi.load_tx_data(word);
		spi.start_transfer();
	}

	void begin_open_transmission() {
		spi.start_transfer();
	}
	void begin_open_data_transmission(uint8_t fifo_size) {
		wait_until_ready();
		spi.disable();
		spi.set_fifo_threshold(fifo_size);
		spi.clear_TXTF_flag(); // needed?
		spi.enable();
		spi.start_transfer();
	}

	void transmit(uint32_t word) {
		wait_until_ready();
		spi.load_tx_data(word);
	}
	void wait_until_xfer_complete() {
		while (!spi.is_tx_complete())
			;
	}
	void end_open_data_transmission() {
		// needed?
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.enable();
	}

	void select_chip(uint32_t chip) {
		if constexpr (ConfT::SpiConf::use_hardware_ss == true)
			return;
		if constexpr (ConfT::SpiConf::use_hardware_ss == false)
			spi.select(chip);
	}
	void unselect_chip(uint32_t chip) {
		if constexpr (ConfT::SpiConf::use_hardware_ss == true)
			return;
		if constexpr (ConfT::SpiConf::use_hardware_ss == false)
			spi.unselect(chip);
	}
	template<uint32_t chip>
	void select_chip() {
		if constexpr (ConfT::SpiConf::use_hardware_ss == true)
			return;
		if constexpr (ConfT::SpiConf::use_hardware_ss == false)
			spi.template select<chip>();
	}
	template<uint32_t chip>
	void unselect_chip() {
		if constexpr (ConfT::SpiConf::use_hardware_ss == true)
			return;
		if constexpr (ConfT::SpiConf::use_hardware_ss == false)
			spi.template unselect<chip>();
	}

	void select_cur_chip() {
		if constexpr (ConfT::SpiConf::use_hardware_ss == true)
			return;
		if constexpr (ConfT::SpiConf::use_hardware_ss == false)
			spi.select(_cur_chip);
	}
	void unselect_cur_chip() {
		if constexpr (ConfT::SpiConf::use_hardware_ss == true)
			return;
		if constexpr (ConfT::SpiConf::use_hardware_ss == false)
			spi.unselect(_cur_chip);
	}

	void unselect_all_chips() {
		auto tmp = _cur_chip;
		spi.unselect(_cur_chip);
		advance_chip_and_channel();
		while (_cur_chip != tmp) {
			spi.unselect(_cur_chip);
			advance_chip_and_channel();
		}
	}

	void advance_chip() {
		if constexpr (ConfT::SpiConf::NumChips == 1)
			return;
		if constexpr (ConfT::SpiConf::NumChips == 2) {
			_cur_chip = 1 - _cur_chip;
			return;
		}
		if constexpr (ConfT::SpiConf::NumChips > 2) {
			if (++_cur_chip >= ConfT::SpiConf::NumChips)
				_cur_chip = 0;
		}
	}

	void advance_chip_and_channel() {
		if constexpr (ConfT::SpiConf::NumChips == 1) {
			advance_channel();
			return;
		}
		if constexpr (ConfT::SpiConf::NumChips == 2) {
			_cur_chip = 1 - _cur_chip;
			if (_cur_chip == 0)
				advance_channel();
			return;
		}
		if constexpr (ConfT::SpiConf::NumChips > 2) {
			if (++_cur_chip >= ConfT::SpiConf::NumChips) {
				_cur_chip = 0;
				advance_channel();
			}
		}
	}

	void advance_channel() {
		if constexpr (ConfT::NumChannelsPerChip == 1)
			return;

		if constexpr (ConfT::NumChannelsPerChip == 2)
			_cur_chan = 1 - _cur_chan;

		_cur_chan++;
		if (_cur_chan >= ConfT::NumChannelsPerChip)
			_cur_chan = 0;
	}
};

