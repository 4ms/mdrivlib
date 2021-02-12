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
		// 863ns: un/select(cur_chip)
		// 748ns: GPIOG->BSRR = cur_chip ? (1 << 10) : (1 << 12)o
		// /256: at 81kHz: each channel = 20kHz, with OS=16 1.26kHz @ 16bit
		// /128: at 145kHz: 13% load: each channel = 36kHz, OS=16 2.2kHz @ 16bit
		// /64: at 250kHz: 22% load: each channel = 62kHz, OS=16 4kHz @ 16bit

		// Idea: create 4 lambdas (global linkage) each has no branching
		// and set the next lambda at the end of the ISR (will this actually save cycles?)

		// Todo: use hardware NSS, and use ch2sel pin to mux the NSS output to chip 1 or chip 2
		// Then run SPI DMA in duplex mode for 16 packets: 8 with CH1 selected, 8 with CH2 selected.
		// Only one ISR at the end of this 16-packet run (could use FIFO, instead of DMA)
		// ISR toggles select pin and repeats
		InterruptManager::registerISR(ConfT::IRQn, [this]() {
			Debug::Pin3::high();
			if (periph.is_end_of_transfer()) {
				periph.unselect(cur_chip);
				periph.clear_EOT_flag();
				periph.clear_TXTF_flag();
				if (periph.rx_packet_available()) {
					store_reading(periph.received_data());
				}
				advance_chip();
				if (cur_chip == 0)
					advance_channel();
				periph.select(cur_chip);
				periph.load_tx_data(cur_chan == 1 ? SWITCH_TO_CH1 : SWITCH_TO_CH2);
				periph.start_transfer();
			}
			Debug::Pin3::low();
			// Todo: check for OVR flag, UDR flag, MODF, FRE
		});

		periph.configure();

		auto pri = System::encode_nvic_priority(ConfT::priority1, ConfT::priority2);
		NVIC_SetPriority(ConfT::IRQn, pri);
		NVIC_EnableIRQ(ConfT::IRQn);

		init();
	}
	void init() {
		cur_chip = 0;
		cur_chan = 0;
		unselect_all_chips();

		periph.set_tx_data_size(1);
		periph.enable();
		periph.enable_end_of_xfer_interrupt();
	}

	void start() {
		periph.select(cur_chip);
		periph.load_tx_data(SWITCH_TO_CH1);
		periph.start_transfer();
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

	void advance_chip() {
		if constexpr (ConfT::NumChips == 1)
			return;
		if constexpr (ConfT::NumChips == 2) {
			cur_chip = 1 - cur_chip;
			return;
		}

		cur_chip++;
		if (cur_chip >= ConfT::NumChips)
			cur_chip = 0;
	}
	void advance_channel() {
		cur_chan = 1 - cur_chan;
	}

	void unselect_all_chips() {
		auto tmp = cur_chip;
		periph.unselect(cur_chip);
		advance_chip();
		while (cur_chip != tmp) {
			periph.unselect(cur_chip);
			advance_chip();
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

