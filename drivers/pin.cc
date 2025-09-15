#include "drivers/pin.hh"
#include "drivers/arch.hh"
#include "drivers/rcc.hh"

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
	, pin_(static_cast<uint16_t>(1 << (pin & 0x0F)))
	, polarity_(polarity) {
	_init(mode, af, pull, speed, otype);
}
Pin::Pin(const PinDef &other, PinMode mode, PinPull pull, PinPolarity polarity, PinSpeed speed, PinOType otype)
	: port_(other.gpio)
	, pin_(static_cast<uint16_t>(1 << (other.pin & 0x0F)))
	, polarity_(polarity) {
	_init(mode, other.af, pull, speed, otype);
}

void Pin::init(const PinDef &other, PinMode mode, PinPull pull, PinPolarity polarity, PinSpeed speed, PinOType otype) {
	port_ = other.gpio;
	pin_ = static_cast<uint16_t>(1 << (other.pin & 0x0F));
	polarity_ = polarity;
	_init(mode, other.af, pull, speed, otype);
}

void Pin::_init(PinMode mode, uint8_t af, PinPull pull, PinSpeed speed, PinOType otype) {
	if (port_ == GPIO::Unused)
		return;
	RCC_Enable::GPIO::enable(GPIOPort(port_));
	set_mode(mode);
	set_pull(pull);
	set_speed(speed);
	if (mode == PinMode::Alt || mode == PinMode::Output)
		set_otype(otype);
	if (mode == PinMode::Alt)
		set_alt(af);
}

void Pin::high() const {
	if (port_ != GPIO::Unused)
		LL_GPIO_SetOutputPin(GPIOPort(port_), pin_);
}
void Pin::low() const {
	if (port_ != GPIO::Unused)
		LL_GPIO_ResetOutputPin(GPIOPort(port_), pin_);
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
void Pin::set_to(uint32_t v) {
	if (v)
		on();
	else
		off();
}
void Pin::toggle() const {
	if (port_ != GPIO::Unused)
		LL_GPIO_TogglePin(GPIOPort(port_), pin_);
}

bool Pin::read_raw() {
	if (port_ != GPIO::Unused)
		return LL_GPIO_IsInputPinSet(GPIOPort(port_), pin_) ? true : false;
	else
		return false;
}
uint8_t Pin::is_on() {
	return (polarity_ == PinPolarity::Normal) ? read_raw() : !read_raw();
}

void Pin::set_mode(PinMode mode) {
	if (port_ != GPIO::Unused)
		LL_GPIO_SetPinMode(GPIOPort(port_), pin_, HALParam(mode));
}
void Pin::set_speed(PinSpeed speed) {
	if (port_ != GPIO::Unused)
		LL_GPIO_SetPinSpeed(GPIOPort(port_), pin_, HALParam(speed));
}
void Pin::set_pull(PinPull pull) {
	if (port_ != GPIO::Unused)
		LL_GPIO_SetPinPull(GPIOPort(port_), pin_, HALParam(pull));
}
void Pin::set_alt(uint8_t af) {
	if (port_ == GPIO::Unused)
		return;

	if (pin_ >= LL_GPIO_PIN_8)
		LL_GPIO_SetAFPin_8_15(GPIOPort(port_), pin_, af);
	else
		LL_GPIO_SetAFPin_0_7(GPIOPort(port_), pin_, af);
}
void Pin::set_otype(PinOType otype) {
	if (port_ != GPIO::Unused)
		LL_GPIO_SetPinOutputType(GPIOPort(port_), pin_, HALParam(otype));
}
} // namespace mdrivlib
