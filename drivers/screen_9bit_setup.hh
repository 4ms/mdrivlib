#pragma once
#include "drivers/bit_bang.hh"
#include "drivers/bit_bang_spi_conf.hh"
#include "drivers/pin.hh"
#include "util/base_concepts.hh"
#include <span>

namespace mdrivlib
{

struct Screen9BitPacket {
	enum class Type { Data, Cmd, DelayMS } type;
	uint8_t payload;
};

template<Derived<BitBangSpiTxConf> ConfT>
class LTDCSerial9BitSetup {

	mdrivlib::BitBang9Bit<ConfT> writer;
	Pin reset_pin;

public:
	LTDCSerial9BitSetup(PinDef reset_pin)
		: reset_pin{reset_pin, PinMode::Output} {
	}

	void setup_driver_chip(std::span<const Screen9BitPacket> packets) {
		writer.init_pins();
		// writer.test_pins();
		_reset();
		_init_display_driver(packets);
	}

private:
	void _init_display_driver(std::span<const Screen9BitPacket> packets) {
		for (auto p : packets) {
			if (p.type == Screen9BitPacket::Type::Data)
				writer.send((1 << 8) | p.payload);

			else if (p.type == Screen9BitPacket::Type::Cmd)
				writer.send((0 << 8) | p.payload);

			else if (p.type == Screen9BitPacket::Type::DelayMS)
				HAL_Delay(p.payload);
		}
	}

	void _reset() {
		HAL_Delay(200);
		reset_pin.high();
		HAL_Delay(200);
		reset_pin.low();
		HAL_Delay(200);
		reset_pin.high();
		HAL_Delay(200);
	}
};
} // namespace mdrivlib
