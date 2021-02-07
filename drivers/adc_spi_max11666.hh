#pragma once
#include "arch.hh"
#include "filter.hh"
#include "interrupt.hh"
#include "spi.hh"
#include "spi_config_struct.hh"
#include "system.hh"

// ConfT: Requires ConfT::PeriphNum and ConfT::NumChips
// PostFilterT: Requires void add_val(T) and T val()
template<typename ConfT, typename PostFilterT = NoFilter>
struct AdcSpi_MAX11666 {
	static constexpr unsigned NumChannelsPerChip = 2;

	AdcSpi_MAX11666(const ConfT &conf)
		: periph{conf}
		, cur_chan{0}
		, cur_chip{0} //
	{
		InterruptManager::registerISR(conf.IRQn, [this]() {
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
			// check for OVR flag, UDR flag, MODF, FRE
		});

		periph.configure();

		auto pri = System::encode_nvic_priority(conf.priority1, conf.priority2);
		NVIC_SetPriority(conf.IRQn, pri);
		NVIC_EnableIRQ(conf.IRQn);

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
		// val[buffer_idx()] = reading >> 2;
		// val[cur_chip * 2 + cur_chan] = reading >> 2;
	}

	uint16_t get_val(int chan) {
		return postfilter[chan].val();
		// return val[chan];
	}

private:
	SpiPeriph<ConfT> periph;
	PostFilterT postfilter[ConfT::NumChips * NumChannelsPerChip];
	// uint16_t val[ConfT::NumChips * 2];
	unsigned cur_chan;
	unsigned cur_chip;

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

