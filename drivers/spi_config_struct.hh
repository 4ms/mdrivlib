#pragma once
#include "interrupt.hh"
#include "pin.hh"

namespace mdrivlib
{
using mdrivlib::PinNum;
using mdrivlib::PinAF;
enum SpiDataDir { Duplex, TXOnly, RXOnly, HalfDuplex };

struct DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 1; // 1 ==> SPI1, 2 ==> SPI2, etc
	static constexpr uint16_t NumChips = 1;
	static constexpr bool is_controller = true; // aka "master"
	static constexpr IRQn_Type IRQn = SPI1_IRQn;
	static constexpr uint16_t priority1 = 3;
	static constexpr uint16_t priority2 = 3;
	static constexpr PinDef SCLK = {GPIO::Unused, PinNum::_0};
	static constexpr PinDef COPI = {GPIO::Unused, PinNum::_0};
	static constexpr PinDef CIPO = {GPIO::Unused, PinNum::_0};
	static constexpr PinDef CS0 = {GPIO::Unused, PinNum::_0};
	static constexpr PinDef CS1 = {GPIO::Unused, PinNum::_0};
	static constexpr PinDef CS2 = {GPIO::Unused, PinNum::_0};
	static constexpr PinDef CS3 = {GPIO::Unused, PinNum::_0};
	static constexpr bool use_hardware_ss = false;
	static constexpr uint16_t clock_division = 64;
	static constexpr uint16_t data_size = 16;
	static constexpr SpiDataDir data_dir = SpiDataDir::Duplex;

	// SPI Mode 00 = false, false
	static constexpr bool clock_high_when_idle = false;
	static constexpr bool second_clk_transition_captures_data = false;

	static constexpr bool SS_active_high = false;
	static constexpr bool pulse_hardware_ss = false;

	static constexpr bool EnableCRC = false;
	static constexpr uint16_t CRCSize = 0;
	static constexpr uint32_t CRCPoly = 0;

	static constexpr uint32_t NumClocksToggleSSInterData = 0;
	static constexpr uint32_t NumClocksDelayBeforeData = 0;

	static constexpr uint8_t FifoThreshold = 1;
};
} // namespace mdrivlib
