#pragma once
#include "arch.hh"
#include "debug.hh"
#include "interrupt.hh"
#include "spi.hh"
#include "spi_config_struct.hh"
#include "system.hh"

// ConfT: Required to be derived from DefaultSpiConf
// PostFilterT: Requires void add_val(T) and T val()
template<typename ConfT, typename PostFilterT>
struct AdcSpi_MAX11666 {
	static constexpr unsigned NumChannelsPerChip = 2;

	AdcSpi_MAX11666() {
		// Note: odd behavior: SPI CLK starts ~1.5us after CSTART bit is set. Why? It's a bottleneck.
		// Todo: is there a way to use this SPI periph more directly, without FIFO? (faster response time for TX)

		periph.configure();

		auto pri = System::encode_nvic_priority(ConfT::priority1, ConfT::priority2);
		NVIC_SetPriority(ConfT::IRQn, pri);
		NVIC_EnableIRQ(ConfT::IRQn);

		_init();
	}

private:
	void _init() {
		cur_chip = 0;
		cur_chan = 0;
		unselect_all_chips();

		periph.set_tx_message_size(1);
		periph.enable();
	}

public:
	void start_circular_mode() {
		// Todo: Try this: use hardware NSS, and use ch2sel pin to mux the NSS output to chip 1 or chip 2
		// Then run SPI DMA in duplex mode for 16 packets: 8 with CH1 selected, 8 with CH2 selected.
		// Only one ISR at the end of this 16-packet run (could use FIFO, instead of DMA)
		// ISR toggles select pin and repeats

		// 691ns with constexpr ConfT
		// /256: at 81kHz: each channel = 20kHz, with OS=16 1.26kHz @ 16bit
		// /128: at 145kHz: 13% load: each channel = 36kHz, OS=16 2.2kHz @ 16bit
		// /64: at 250kHz: 22% load: each channel = 62kHz, OS=16 4kHz @ 16bit
		periph.enable_end_of_xfer_interrupt();
		InterruptManager::registerISR(ConfT::IRQn, [this]() {
			start();
			// Todo: check for OVR flag, UDR flag, MODF, FRE
		});
	}

	void start() {
		// Debug::Pin2::high();
		if (periph.is_end_of_transfer()) {
			unselect_cur_chip();
			periph.clear_EOT_flag();
			periph.clear_TXTF_flag();
			if (periph.rx_packet_available()) {
				store_reading(periph.received_data());
			}
			advance_chip_and_channel();
		}
		select_cur_chip();
		periph.load_tx_data(cur_chan == 1 ? SWITCH_TO_CH1 : SWITCH_TO_CH2);
		periph.start_transfer();
		// Debug::Pin2::low();
	}

	void store_reading(uint16_t reading) {
		postfilter[buffer_idx()].add_val(reading >> 2);
	}

	uint16_t get_val(int chan) {
		return postfilter[chan].val();
	}

private:
	SpiPeriph<ConfT> periph;
	PostFilterT postfilter[ConfT::NumChips * NumChannelsPerChip];
	unsigned cur_chan = 0;
	unsigned cur_chip = 0;

	void select_cur_chip() {
		if constexpr (ConfT::use_hardware_ss == true)
			return;
		if constexpr (ConfT::use_hardware_ss == false)
			periph.select(cur_chip);
	}
	void unselect_cur_chip() {
		if constexpr (ConfT::use_hardware_ss == true)
			return;
		if constexpr (ConfT::use_hardware_ss == false)
			periph.unselect(cur_chip);
	}
	void advance_chip_and_channel() {
		if constexpr (ConfT::NumChips == 1) {
			advance_channel();
			return;
		}
		if constexpr (ConfT::NumChips == 2) {
			cur_chip = 1 - cur_chip;
			if (cur_chip == 0)
				advance_channel();
			return;
		}

		cur_chip++;
		if (cur_chip >= ConfT::NumChips) {
			cur_chip = 0;
			advance_channel();
		}
	}
	void advance_channel() {
		cur_chan = 1 - cur_chan;
	}

	void unselect_all_chips() {
		auto tmp = cur_chip;
		periph.unselect(cur_chip);
		advance_chip_and_channel();
		while (cur_chip != tmp) {
			periph.unselect(cur_chip);
			advance_chip_and_channel();
		}
	}

	constexpr unsigned buffer_idx() const {
		return cur_chip * 2 + cur_chan;
	}
	enum Max11666Commands {
		SWITCH_TO_CH1 = 0x00FF,
		CONTINUE_READING_CH1 = 0x0000,
		SWITCH_TO_CH2 = 0xFF00,
		CONTINUE_READING_CH2 = 0xFFFF,
	};
};

