#pragma once
#include "drivers/debounced_switch.hh"
#include "drivers/pin.hh"
#include "drivers/rotary_tables.hh"
#include <array>
#include <cstdint>

namespace mdrivlib
{

// A runtime-initialized version of RotaryEnc, suitable for being in a container with other Rotaries
class RotaryEncoder {
public:
	RotaryEncoder(const PinDef &pinA, const PinDef &pinB, RotaryStepSize step_size)
		: pinA{pinA, PinMode::Input, PinPull::Up, PinPolarity::Inverted}
		, pinB{pinB, PinMode::Input, PinPull::Up, PinPolarity::Inverted}
		, StateTable(step_size == RotaryStepSize::RotaryHalfStep ? RotaryEncDetails::RotaryStateTable<RotaryHalfStep>
																 : RotaryEncDetails::RotaryStateTable<RotaryFullStep>) {
	}

	void update() {
		uint8_t pinstate = pinA.is_on() ? 0b00 : 0b10;
		pinstate += pinB.is_on() ? 0b00 : 0b01;
		state_ = StateTable[state_ & 0xf][pinstate];
		uint8_t motion = state_ & (DIR_CW | DIR_CCW);
		if (motion == DIR_CW) {
			position_++;
		}
		if (motion == DIR_CCW) {
			position_--;
		}
	}

	int32_t read() {
		auto tmp = position_;
		position_ = 0;
		return tmp;
	}

private:
	Pin pinA;
	Pin pinB;
	uint8_t state_{};
	int32_t position_{};
	const RotaryEncDetails::StateTableType &StateTable;

	enum RotaryDir : uint8_t {
		DIR_NONE = 0x00,
		DIR_CW = 0x10,
		DIR_CCW = 0x20,
	};
};

} // namespace mdrivlib
