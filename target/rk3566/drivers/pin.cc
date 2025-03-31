#include "pin.hh"
#include "drivers/register_access.hh"

namespace mdrivlib
{

Pin::Pin(GPIO port,
		 uint8_t pin,
		 PinMode mode,
		 uint8_t af,
		 PinPull pull,
		 PinPolarity polarity,
		 PinSpeed speed,
		 PinOType otype)
	: port_(port)
	, pin_(pin)
	, polarity_(polarity) {
	_init(mode, af, pull, speed, otype);
}

Pin::Pin(const PinDef &other, PinMode mode, PinPull pull, PinPolarity polarity, PinSpeed speed, PinOType otype)
	: port_(other.gpio)
	, pin_(other.pin)
	, polarity_(polarity) {
	_init(mode, other.af, pull, speed, otype);
}

void Pin::init(const PinDef &other, PinMode mode, PinPull pull, PinPolarity polarity, PinSpeed speed, PinOType otype) {
	port_ = other.gpio;
	pin_ = other.pin;
	polarity_ = polarity;
	_init(mode, other.af, pull, speed, otype);
}

void Pin::_init(PinMode mode, uint8_t af, PinPull pull, PinSpeed speed, PinOType otype) {
	if (port_ == GPIO::Unused)
		return;
	// CRU gate enable GPIO?
	set_mode(mode);
	set_pull(pull);
	set_speed(speed);
	if (mode == PinMode::Alt || mode == PinMode::Output)
		set_otype(otype);
	if (mode == PinMode::Alt)
		set_alt(af);
}

static volatile Gpio *base_register(GPIO gpio) {
	return reinterpret_cast<volatile Gpio *>(gpio);
}

static bool is_low_reg(uint16_t pin) {
	return (pin < C0);
}
static uint16_t to_low(uint16_t pin) {
	return pin - C0;
}

void Pin::high() const {
	if (is_low_reg(pin_))
		base_register(port_)->data_L = masked_set_bit(pin_);
	else
		base_register(port_)->data_H = masked_set_bit(to_low(pin_));
}

void Pin::low() const {
	if (is_low_reg(pin_))
		base_register(port_)->data_L = masked_clr_bit(pin_);
	else
		base_register(port_)->data_H = masked_clr_bit(to_low(pin_));
}

void Pin::on() const {
	if (polarity_ == PinPolarity::Normal)
		high();
	else
		low();
}
void Pin::off() const {
	if (polarity_ == PinPolarity::Normal)
		low();
	else
		high();
}
void Pin::set_to(uint32_t v) const {
	if (v)
		on();
	else
		off();
}
void Pin::toggle() const {
	if (read_raw())
		low();
	else
		high();
	//TODO
}

bool Pin::read_raw() const {
	return base_register(port_)->ext_port & (1 << pin_);
}
uint8_t Pin::is_on() const {
	return (polarity_ == PinPolarity::Normal) ? read_raw() : !read_raw();
}

void Pin::set_mode(PinMode mode) {
	if (mode == PinMode::Alt) {
		//TODO
	} else {
		if (mode == PinMode::Output) {
			if (is_low_reg(pin_))
				base_register(port_)->dir_L = masked_set_bit(pin_);
			else
				base_register(port_)->dir_H = masked_set_bit(to_low(pin_));
		} else {
			if (is_low_reg(pin_))
				base_register(port_)->dir_L = masked_clr_bit(pin_);
			else
				base_register(port_)->dir_H = masked_clr_bit(to_low(pin_));
		}
	}
}
void Pin::set_speed(PinSpeed speed) {
	//TODO
}
void Pin::set_pull(PinPull pull) {
	//TODO
}
void Pin::set_alt(uint8_t af) {
	//TODO
}
void Pin::set_otype(PinOType otype) {
	//TODO
}

} // namespace mdrivlib
