#pragma once
#include "drivers/pin.hh"
#include "drivers/screen_9bit_packet.hh"
#include "drivers/spi_config_struct.hh"
#include "drivers/spi_transfer.hh"
#include "util/base_concepts.hh"
#include <span>

namespace mdrivlib
{

template<Derived<DefaultSpiConf> ConfT>
class LTDCSerial9BitSpiSetup {

	mdrivlib::SpiTransferDriver<ConfT> writer;
	Pin reset_pin;

public:
	LTDCSerial9BitSpiSetup(PinDef reset_pin)
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
