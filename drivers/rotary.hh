#pragma once
#include "drivers/debounced_switch.hh"
#include "drivers/pin.hh"
#include "drivers/rotary_tables.hh"
#include <array>
#include <cstdint>

namespace mdrivlib
{

namespace RotaryEncDetails
{

template<RotaryStepSize StepSize>
class RotaryFSM {

public:
	RotaryFSM() = default;

	void update(bool state_a, bool state_b) {
		uint8_t pinstate = state_a ? 0b00 : 0b10;
		pinstate += state_b ? 0b00 : 0b01;
		state_ = RotaryStateTable<StepSize>[state_ & 0xf][pinstate];
		uint8_t motion = state_ & (DIR_CW | DIR_CCW);
		if (motion == DIR_CW)
			position_++;
		if (motion == DIR_CCW)
			position_--;
	}

	int32_t read_position() {
		auto tmp = position_;
		position_ = 0;
		return tmp;
	}

private:
	uint8_t state_{0};
	int32_t position_{0};

	enum RotaryDir : uint8_t {
		DIR_NONE = 0x00,
		DIR_CW = 0x10,
		DIR_CCW = 0x20,
	};
};

} // namespace RotaryEncDetails

template<RotaryStepSize StepSize, PinDef PinA, PinDef PinB>
class RotaryEnc {
public:
	RotaryEnc() = default;

	void update() {
		pin_a.update();
		pin_b.update();
		rotary.update(pin_a.is_pressed(), pin_b.is_pressed());
	}

	int32_t read() {
		return rotary.read_position();
	}

private:
	RotaryEncDetails::RotaryFSM<StepSize> rotary;
	DebouncedPin<PinA, PinPolarity::Inverted, 0x0000FFFF, 0xFFFF0000, 0xFFFFFFFF> pin_a;
	DebouncedPin<PinB, PinPolarity::Inverted, 0x0000FFFF, 0xFFFF0000, 0xFFFFFFFF> pin_b;
};

} // namespace mdrivlib
