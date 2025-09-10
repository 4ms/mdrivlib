#pragma once
#include "drivers/parallel_writer.hh"
#include "drivers/pin.hh"
#include "util/base_concepts.hh"
#include <span>

namespace mdrivlib
{

struct Screen9BitPacket {
	enum class Type { Data, Cmd, DelayMS } type;
	uint8_t payload;
};

template<Derived<ParallelWriterConf> ConfT>
class LTDCParallelSetup {

	// static_assert(ConfT::dc.gpio == mdrivlib::GPIO::Unused);

	mdrivlib::ParallelWriter<ConfT> writer;

public:
	void setup_driver_chip(std::span<const Screen9BitPacket> packets) {
		writer.init_pins();
		_reset();
		_init_display_driver(packets);
	}

private:
	void _init_display_driver(std::span<const Screen9BitPacket> packets) {
		writer.start_sequence();

		// TODO: delay?

		for (auto p : packets) {
			if (p.type == Screen9BitPacket::Type::Data)
				writer.send_arg(p.payload);
			else if (p.type == Screen9BitPacket::Type::Cmd)
				writer.send_cmd(p.payload);
			else if (p.type == Screen9BitPacket::Type::DelayMS)
				HAL_Delay(p.payload);
		}

		writer.end_sequence();
	}

	void _reset() {
		Pin reset_pin{ConfT::reset, PinMode::Output};

		reset_pin.high();
		HAL_Delay(10);
		reset_pin.low();
		HAL_Delay(100);
		reset_pin.high();
		HAL_Delay(10);
	}
};
} // namespace mdrivlib
