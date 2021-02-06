#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "pin.hh"
#include "rcc.hh"
#include "spi.hh"
#include "spi_config_struct.hh"
#include "system.hh"

// Requires ConfT::PeriphNum and ConfT::NumChips
// Todo handle >2 chips
template<typename ConfT>
struct AdcSpi_MAX11666 {
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
				// chip0, chan0: SWITCH_TO_CH2
				// chip1, chan0: SWITCH_TO_CH2
				// chip0, chan1: SWITCH_TO_CH1
				// chip1, chan1: SWITCH_TO_CH1
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
		periph.unselect(cur_chip);
		periph.unselect(1 - cur_chip);

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
		val[cur_chip * 2 + cur_chan] = reading >> 2;
	}
	void store_reading(int chip, int chan, uint16_t reading) {
		val[chip * 2 + chan] = reading >> 2;
	}

	uint16_t get_val(int chan) {
		return val[chan];
	}

private:
	SpiPeriph<ConfT> periph;
	uint16_t val[ConfT::NumChips * 2];
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

	enum Max11666Commands {
		SWITCH_TO_CH1 = 0x00FF,
		CONTINUE_READING_CH1 = 0x0000,
		SWITCH_TO_CH2 = 0xFF00,
		CONTINUE_READING_CH2 = 0xFFFF,
	};
	// static constexpr uint32_t cmd_seq[4] = {
	// 	SWITCH_TO_CH1,
	// 	CONTINUE_READING_CH1,
	// 	SWITCH_TO_CH2,
	// 	CONTINUE_READING_CH2,
	// };
};

