#pragma once
#include "drivers/gpio.hh"
#include <cstdint>

namespace mdrivlib
{

struct PinDef {
	GPIO gpio{GPIO::Unused};
	PinNum pin;
	PinAF af;
};

class Pin {
public:
	Pin() = default;

	// Pin{GPIO::A, 10, PinMode::Input, ...};
	Pin(GPIO port,
		uint8_t pin,
		PinMode mode,
		uint8_t af = 0,
		PinPull pull = PinPull::None,
		PinPolarity polarity = PinPolarity::Normal,
		PinSpeed speed = PinSpeed::High,
		PinOType otype = PinOType::PushPull);

	Pin(const PinDef &other,
		PinMode mode,
		PinPull pull = PinPull::None,
		PinPolarity polarity = PinPolarity::Normal,
		PinSpeed speed = PinSpeed::High,
		PinOType otype = PinOType::PushPull);

	void init(const PinDef &other,
			  PinMode mode,
			  PinPull pull = PinPull::None,
			  PinPolarity polarity = PinPolarity::Normal,
			  PinSpeed speed = PinSpeed::High,
			  PinOType otype = PinOType::PushPull);

	void high() const;
	void low() const;
	void on() const;
	void off() const;
	void set_to(uint32_t v) const;
	void toggle() const;
	bool read_raw() const;
	uint8_t is_on() const;
	void set_mode(PinMode mode);
	void set_speed(PinSpeed speed);
	void set_pull(PinPull pull);
	void set_alt(uint8_t af);
	void set_otype(PinOType otype);

private:
	GPIO port_;
	uint16_t pin_;
	PinPolarity polarity_;

	void _init(PinMode mode, uint8_t af, PinPull pull, PinSpeed speed, PinOType otype);
};

} // namespace mdrivlib
