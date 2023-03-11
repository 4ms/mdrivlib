#pragma once
#include "drivers/pin.hh"
#include <cstdint>

struct UartConf {
	uint32_t base_addr;
	mdrivlib::PinDef TXPin;
	mdrivlib::PinDef RXPin;
	enum class Mode { TX, RX, TXRX } mode = Mode::TXRX;
	uint32_t baud = 115200;
	uint32_t wordlen = 8;
	enum class Parity { None, Odd, Even } parity = Parity::None;
	enum class StopBits { _0_5, _1, _1_5, _2 } stopbits = StopBits::_1;
	//TODO: CLK polarity, phase, lastbit
	//TODO: HWFlowCtl, OverSampling, OneBitSampling
};
