#pragma once
#include "drivers/bit_bang_spi_conf.hh"
#include "drivers/pin.hh"
#include "util/base_concepts.hh"

namespace mdrivlib
{

template<Derived<BitBangSpiTxConf> ConfT>
struct BitBang9Bit {
	Pin data;
	Pin cs;
	Pin clk;

	void init() {
		data.init(ConfT::data, mdrivlib::PinMode::Output);
		cs.init(ConfT::chip_sel, mdrivlib::PinMode::Output);
		clk.init(ConfT::clock, mdrivlib::PinMode::Output, {}, ConfT::clk_polarity);

		cs.high();

		clk.off();

		// Why is this needed?
		// Setting a fixed delay does not work, it has to be between data pin toggles
		// Stepping through the code here always works, too
		// The clock rising edge position relative to the DE pulse wanders anyways
		__BKPT();
		data.low();
		HAL_Delay(200);
		data.high();
		HAL_Delay(200);
		data.low();
		HAL_Delay(300);
	}

	void delay(unsigned cycles) {
		volatile int x = cycles;
		while (x)
			x = x - 1;
	}

	void transmit_blocking(uint16_t packet9bit) {
		cs.low();
		delay(ConfT::ChipSelectSetupTime);

		data.set_to(packet9bit & (1 << 8));
		delay(ConfT::DataSetupTime);

		clk.off();
		delay(ConfT::ClockLowTime);

		clk.on();
		delay(ConfT::ClockHighTime);

		for (unsigned i = 0; i < 8; i++) {
			clk.off();
			delay(ConfT::ClockLowTime);

			data.set_to(packet9bit & (1 << (7 - i)));
			delay(ConfT::DataSetupTime);

			clk.on();
			delay(ConfT::ClockHighTime);
		}

		cs.high();
	}

	void select_cur_chip() {
		cs.low();
	}

	void unselect_cur_chip() {
		cs.high();
	}

	void test_pins() {
		cs.low();
		cs.high();
		cs.low();
		cs.high();

		clk.low();
		clk.high();
		clk.low();

		data.low();
		data.high();
		data.low();
	}
};
} // namespace mdrivlib
