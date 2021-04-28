#include "drivers/pinchange.hh"
#include "drivers/exti.hh"
#include "drivers/interrupt.hh"
#include "rcc.hh"

PinChangeInterrupt::PinChangeInterrupt() {
}

PinChangeInterrupt::PinChangeInterrupt(const PinChangeConfig &config, std::function<void(void)> &&func)
	: task_func{std::move(func)}
	, _pin{config.pin} {
	_init(config);
}

void PinChangeInterrupt::init(const PinChangeConfig &config, std::function<void(void)> &&func) {
	task_func = std::move(func);
	_pin = config.pin;
	_init(config);
}

void PinChangeInterrupt::_init(const PinChangeConfig &config) {
	target::RCC_Enable::SYSCFG_::set();
	auto port = config.port == GPIO::A ? target::EXTI_::PortA
			  : config.port == GPIO::B ? target::EXTI_::PortB
			  : config.port == GPIO::C ? target::EXTI_::PortC
			  : config.port == GPIO::D ? target::EXTI_::PortD
			  : config.port == GPIO::E ? target::EXTI_::PortE
			  : config.port == GPIO::F ? target::EXTI_::PortF
			  : config.port == GPIO::G ? target::EXTI_::PortG
			  : config.port == GPIO::H ? target::EXTI_::PortH
			  : config.port == GPIO::I ? target::EXTI_::PortI
			  : config.port == GPIO::J ? target::EXTI_::PortJ
									   : target::EXTI_::PortK;
	if (config.pin == 1)
		target::EXTI_::Pin1::write(port);
	if (config.pin == 2)
		target::EXTI_::Pin2::write(port);
	if (config.pin == 3)
		target::EXTI_::Pin3::write(port);
	if (config.pin == 4)
		target::EXTI_::Pin4::write(port);
	if (config.pin == 5)
		target::EXTI_::Pin5::write(port);
	if (config.pin == 6)
		target::EXTI_::Pin6::write(port);
	if (config.pin == 7)
		target::EXTI_::Pin7::write(port);
	if (config.pin == 8)
		target::EXTI_::Pin8::write(port);
	if (config.pin == 9)
		target::EXTI_::Pin9::write(port);
	if (config.pin == 10)
		target::EXTI_::Pin10::write(port);
	if (config.pin == 11)
		target::EXTI_::Pin11::write(port);
	if (config.pin == 12)
		target::EXTI_::Pin12::write(port);
	if (config.pin == 13)
		target::EXTI_::Pin13::write(port);
	if (config.pin == 14)
		target::EXTI_::Pin14::write(port);
	if (config.pin == 15)
		target::EXTI_::Pin15::write(port);

	// Todo: use set/clear, so we need target::EXTI_::RTSR_Pin<4>
	if (config.on_rising_edge)
		target::EXTI_RTSR1::write(1 << _pin);
	else {
		auto tmp = target::EXTI_RTSR1::read() & ~(1 << _pin);
		target::EXTI_RTSR1::write(tmp);
	}

	if (config.on_falling_edge)
		target::EXTI_FTSR1::write(1 << _pin);
	else {
		auto tmp = target::EXTI_FTSR1::read() & ~(1 << _pin);
		target::EXTI_FTSR1::write(tmp);
	}

	auto irqn = config.pin >= 10 ? EXTI15_10_IRQn
			  : config.pin >= 5	 ? EXTI9_5_IRQn
			  : config.pin == 0	 ? EXTI0_IRQn
			  : config.pin == 1	 ? EXTI1_IRQn
			  : config.pin == 2	 ? EXTI2_IRQn
			  : config.pin == 3	 ? EXTI3_IRQn
								 : EXTI4_IRQn;
	InterruptManager::registerISR(irqn, config.priority1, config.priority2, [&]() {
		if (target::EXTI_PR1::read() == (1 << _pin)) {
			target::EXTI_PR1::write(1 << _pin);
			task_func();
		}
	});
}

void PinChangeInterrupt::start() {
	target::EXTI_IMR1::write(1 << _pin);
}
void PinChangeInterrupt::stop() {
	auto tmp = target::EXTI_IMR1::read() & ~(1 << _pin);
	target::EXTI_IMR1::write(tmp);
}
