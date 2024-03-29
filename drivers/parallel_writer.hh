#pragma once
#include "drivers/parallel_writer_conf.hh"
#include "drivers/pin.hh"
#include "util/base_concepts.hh"
#include "util/countzip.hh"
#include "util/zip.hh"

namespace mdrivlib
{

template<Derived<ParallelWriterConf> ConfT>
struct ParallelWriter {
	std::array<Pin, ConfT::BusWidth> datapin;
	Pin cs;
	Pin wrx;
	Pin dc;

	void init_pins() {
		for (auto [pin, PinConf] : zip(datapin, ConfT::data)) {
			pin.init(PinConf, mdrivlib::PinMode::Output);
		}
		cs.init(ConfT::chip_sel, mdrivlib::PinMode::Output);
		wrx.init(ConfT::write_latch, mdrivlib::PinMode::Output);
		dc.init(ConfT::datacmd_sel, mdrivlib::PinMode::Output);

		cs.high();
		wrx.high();
	}

	void send_cmd_with_args(size_t cmd, auto... data) {
		start_sequence();
		send_cmd(cmd);
		dc.high();
		(_send_data(data), ...);
		end_sequence();
	}

	void start_sequence() {
		cs.low();
	}

	void send_cmd(size_t cmd) {
		dc.low();
		_send_data(cmd);
	}

	void send_arg(size_t d) {
		dc.high();
		_send_data(d);
	}

	void end_sequence() {
		cs.high();
	}

private:
	void _send_data(size_t data) {
		wrx.low();
		for (auto [i, pin] : enumerate(datapin))
			pin.set_to(data & (1 << i));

		volatile int delay = ConfT::DataSetupTime;
		while (delay)
			delay = delay - 1;

		wrx.high();

		delay = ConfT::WriteLatchAfterDelay;
		while (delay)
			delay = delay - 1;
	}

	void _test_pins() {
		cs.low();
		cs.high();
		cs.low();

		dc.low();
		dc.high();
		dc.low();

		wrx.low();
		wrx.high();
		wrx.low();

		for (auto &pin : datapin) {
			pin.low();
			pin.high();
			pin.low();
		}
	}
};
} // namespace mdrivlib
