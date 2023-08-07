#pragma once
#include "drivers/pin.hh"
#include "util/math.hh"
#include "util/zip.hh"
#include <array>
#include <optional>

template<unsigned NumInputChips = 0, unsigned NumOutputChips = 0, unsigned ChannelsPerChip = 8>
struct MuxedIOConfig {
	static_assert(NumInputChips <= 4);
	static_assert(NumOutputChips <= 4);
	static_assert(ChannelsPerChip >= 4 && MathTools::is_power_of_2(ChannelsPerChip));
	static constexpr uint32_t ChanPerChip = ChannelsPerChip;

	std::array<mdrivlib::PinDef, NumInputChips> InputChipPins;
	std::array<mdrivlib::PinDef, NumOutputChips> OutputChipPins;
	std::array<mdrivlib::PinDef, MathTools::Log2<ChannelsPerChip>::val> SelectPins;
};

template<MuxedIOConfig Conf>
struct MuxedIO {

	MuxedIO() {
		for (auto [pin, pin_def] : zip(read_pins, Conf.InputChipPins))
			pin.init(pin_def, mdrivlib::PinMode::Input, mdrivlib::PinPull::None);

		for (auto [pin, pin_def] : zip(write_pins, Conf.OutputChipPins))
			pin.init(pin_def, mdrivlib::PinMode::Output);

		for (auto [pin, pin_def] : zip(sel_pins, Conf.SelectPins))
			pin.init(pin_def, mdrivlib::PinMode::Output);
	}

	// Steps to the next mux channel, reading and writing as needed.
	// Call this at a fixed frequency
	// Returns the read value only after all channels have been read.
	// Otherwise returns nullopt.
	// TODO: wrap `data` into a structure so that we can access by chip, or by bit?
	// ... right now data is packed with LSByte being chip 0, which is not obvious
	std::optional<uint32_t> step(uint32_t data) {

		// Turn all write pins off so we don't accidentally write a 1 to the previous mux channel
		// Or to a channel that gets selected while select() is changing select pins
		for (auto &pin : write_pins)
			pin.off();

		// Select the next channel
		select(cur_chan);

		// Write a 1 or 0 to the MUXed outputs
		for (auto &pin : write_pins)
			pin.set_to(data & (1 << cur_chan));

		// Read each input chip, putting the result into the right bit
		for (unsigned chipnum = 0; auto &pin : read_pins) {
			if (pin.read_raw()) {
				running_read |= (1 << cur_chan) << (8U * chipnum);
			}
			chipnum++;
		}

		cur_chan++;
		if (cur_chan >= Conf.ChanPerChip) {
			cur_chan = 0;
			auto read = running_read;
			running_read = 0;
			return read;
		} else
			return std::nullopt;
	}

private:
	void select(uint32_t chan) {
		for (unsigned bit = 0; auto &pin : sel_pins) {
			pin.set_to(chan & (1 << bit));
			bit++;
		}
	}

	std::array<mdrivlib::Pin, Conf.InputChipPins.size()> read_pins;
	std::array<mdrivlib::Pin, Conf.OutputChipPins.size()> write_pins;
	std::array<mdrivlib::Pin, Conf.SelectPins.size()> sel_pins;

	uint32_t cur_chan = 0;
	uint32_t running_read = 0;
};
