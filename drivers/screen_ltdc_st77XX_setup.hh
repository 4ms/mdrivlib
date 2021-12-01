#pragma once
#include "debug.hh"
#include "drivers/ltdc_screen_config_struct.hh"
#include "drivers/parallel_writer.hh"
#include "drivers/pin.hh"
#include "drivers/spi_screen_ST77XX.hh"
#include "util/base_concepts.hh"
#include <span>

namespace mdrivlib
{
template<Derived<ParallelWriterConf> ConfT>
class ST77XXParallelSetup {

	mdrivlib::ParallelWriter<ConfT> writer;

public:
	void setup_driver_chip(std::span<const ST77XX::InitCommand> cmds) {
		writer.init_pins();
		_reset();
		_init_display_driver(cmds);
	}

private:
	void _init_display_driver(std::span<const ST77XX::InitCommand> cmds) {
		writer.start_sequence();
		for (auto &c : cmds) {
			writer.send_cmd(c.cmd);
			int numArgs = c.num_args;
			uint32_t args = c.args;
			while (numArgs--) {
				writer.send_arg(args & 0x000000FF);
				args >>= 8;
			}
			if (c.delay_ms)
				HAL_Delay(c.delay_ms);
		}
		writer.end_sequence();
	}

	void _reset() {
		Pin reset_pin{ConfT::reset, PinMode::Output};
		reset_pin.high();
		volatile int i = 10000; //10000 = high for 123us
		while (i)
			i = i - 1;
		reset_pin.low();
		i = 1000; //1000 = low for 12.6us
		while (i)
			i = i - 1;
		reset_pin.high();
	}
};
} // namespace mdrivlib
