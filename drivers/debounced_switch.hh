#include "pin.hh"
#include "util/debouncer.hh"
#include <stdint.h>

struct DebouncedSwitch : public Debouncer<0x0000FFFF, 0xFFFF0000> {
	DebouncedSwitch(GPIO port, uint8_t pin_num, PinPolarity polarity = PinPolarity::Inverted)
		: pin_(port,
			   pin_num,
			   PinMode::Input,
			   LL_GPIO_AF_0,
			   polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None,
			   polarity) {}

	void update() {
		unsigned pin_state = pin_.is_on() ? 1 : 0;
		register_state(pin_state);
	}

private:
	Pin pin_;
};

