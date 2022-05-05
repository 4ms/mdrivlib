#pragma once
#include "debounced_switch.hh"
#include "pin.hh"
#include <cstdint>

namespace mdrivlib
{
enum RotaryStepSize { RotaryHalfStep = 0, RotaryFullStep = 6 };

template<RotaryStepSize StepSize>
class RotaryFSM {
public:
	RotaryFSM() = default;

	void update(bool state_a, bool state_b) {
		uint8_t pinstate = state_a ? 0b00 : 0b10;
		pinstate += state_b ? 0b00 : 0b01;
		state_ = StateTable[state_ & 0xf][pinstate];
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
	static const uint8_t StateTable[7][4];

	enum RotaryDir : uint8_t {
		DIR_NONE = 0x00,
		DIR_CW = 0x10,
		DIR_CCW = 0x20,
	};
};

template<RotaryStepSize StepSize, PinNoInit PinA, PinNoInit PinB>
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
	RotaryFSM<StepSize> rotary;
	DebouncedPin<PinA.gpio, PinA.pin, PinPolarity::Inverted, 0x0000FFFF, 0xFFFF0000, 0xFFFFFFFF> pin_a;
	DebouncedPin<PinB.gpio, PinB.pin, PinPolarity::Inverted, 0x0000FFFF, 0xFFFF0000, 0xFFFFFFFF> pin_b;
};

// TODO: remove this and change unit tests to above classes
class RotaryBase {
public:
	RotaryBase(GPIO portA, uint8_t pin_numA, GPIO portB, uint8_t pin_numB);
	void update(const uint32_t table_select);
	int32_t read_position();

private:
	Pin pinA;
	Pin pinB;
	uint8_t state_;
	int32_t position_;
	static const uint8_t StateTable[13][4];
};

template<RotaryStepSize StepSize>
class RotaryEncoder {
public:
	RotaryEncoder(GPIO portA, uint8_t pin_numA, GPIO portB, uint8_t pin_numB)
		: rotary{portA, pin_numA, portB, pin_numB} {
	}
	void update() {
		rotary.update(StepSize);
	}
	int32_t read() {
		return rotary.read_position();
	}

private:
	RotaryBase rotary;
};

} // namespace mdrivlib
