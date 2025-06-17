#pragma once
#include "drivers/ltdc_screen_config_struct.hh"
#include "drivers/parallel_writer.hh"
#include "drivers/pin.hh"
#include "util/base_concepts.hh"
#include <span>

namespace mdrivlib
{

struct ScreenInitCommand {
	uint8_t cmd;
	uint8_t num_args = 0;
	uint16_t delay_ms = 0;
	uint8_t args[8];
	// std::span<const uint8_t> args; // FixMe: std::span<uint8_t> or std::span<const uint8_t> gives error: No matching
	// constructor
};

template<Derived<ParallelWriterConf> ConfT>
class LTDCParallelSetup {

	mdrivlib::ParallelWriter<ConfT> writer;

public:
	void setup_driver_chip(std::span<const ScreenInitCommand> cmds) {
		writer.init_pins();
		_reset();
		_init_display_driver(cmds);
	}

private:
	void _init_display_driver(std::span<const ScreenInitCommand> cmds) {
		writer.start_sequence();
		for (auto &c : cmds) {
			writer.send_cmd(c.cmd);
			for (int i = 0; i < c.num_args; i++) {
				writer.send_arg(c.args[i]);
			}
			if (c.delay_ms)
				HAL_Delay(c.delay_ms);
		}
		writer.end_sequence();
	}

	void _reset() {
		Pin reset_pin{ConfT::reset, PinMode::Output};

		// reset_pin.high();
		// volatile int i = 10000; //10000 = high for 123us
		// while (i)
		// 	i = i - 1;
		// reset_pin.low();
		// i = 1000; //1000 = low for 12.6us
		// while (i)
		// 	i = i - 1;
		// reset_pin.high();

		reset_pin.high();
		HAL_Delay(1);
		reset_pin.low();
		HAL_Delay(10);
		reset_pin.high();
		HAL_Delay(120);
	}
};
} // namespace mdrivlib
