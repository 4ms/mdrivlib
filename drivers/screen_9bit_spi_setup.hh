#pragma once
#include "drivers/pin.hh"
#include "drivers/screen_9bit_packet.hh"
#include <span>

namespace mdrivlib
{

// Sets up an LCD screen with 9-bit SPI
// Caller provides the Pin Driver class.
//
// TODO: set reset timing in ConfT
// Includes reset pin
template<typename PinDriverT>
class Screen9BitSpiSetup {

	PinDriverT writer;
	Pin reset_pin;

public:
	Screen9BitSpiSetup(PinDef reset_pin)
		: reset_pin{reset_pin, PinMode::Output} {
	}

	void setup_driver_chip(std::span<const Screen9BitPacket> packets) {
		writer.init();
		// writer.test_pins();
		_reset();
		_init_display_driver(packets);
	}

private:
	void _init_display_driver(std::span<const Screen9BitPacket> packets) {
		for (auto p : packets) {
			if (p.action == Screen9BitPacket::Type::Data) {
				writer.select_cur_chip();
				writer.transmit_blocking((1 << 8) | p.payload);
				writer.unselect_cur_chip();
			}

			else if (p.action == Screen9BitPacket::Type::Cmd)
			{
				writer.select_cur_chip();
				writer.transmit_blocking((0 << 8) | p.payload);
				writer.unselect_cur_chip();
			}

			else if (p.action == Screen9BitPacket::Type::DelayMS)
				HAL_Delay(p.payload);
		}
	}

	void _reset() {
		HAL_Delay(1);
		reset_pin.high();
		HAL_Delay(1);
		reset_pin.low();
		HAL_Delay(10);
		reset_pin.high();
		HAL_Delay(10);
	}
};
} // namespace mdrivlib
