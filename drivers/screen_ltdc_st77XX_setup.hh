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
	template<Derived<LTDCScreenConf> ScreenConfT>
	void setup_driver_chip() {
		writer.init_pins();
		auto cmds = ST77XX::ST7789RGBInit<ScreenConfT>::cmds;
		_init_display_driver(cmds);
	}

private:
	GCC_OPTIMIZE_OFF
	void _init_display_driver(std::span<ST77XX::InitCommand> cmds) {
		writer.start_sequence();
		for (auto &c : cmds) {
			writer.send_cmd(c.cmd);
			int numArgs = c.num_args;
			uint32_t args = c.args;
			while (numArgs--) {
				writer.send_arg(args & 0xFF);
				args >>= 8;
			}
			if (c.delay_ms)
				HAL_Delay(c.delay_ms);
		}
		writer.end_sequence();
	}
};
} // namespace mdrivlib
