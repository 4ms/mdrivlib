#include "drivers/pinchange.hh"
#include "drivers/interrupt.hh"
#include "rcc.hh"
// #include "syscfg.hh"

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
	// auto port = config.port == GPIO::A ? target::SYSCFG_EXTI::PortA
	// 		  : config.port == GPIO::B ? target::SYSCFG_EXTI::PortB
	// 		  : config.port == GPIO::C ? target::SYSCFG_EXTI::PortC
	// 		  : config.port == GPIO::D ? target::SYSCFG_EXTI::PortD
	// 		  : config.port == GPIO::E ? target::SYSCFG_EXTI::PortE
	// 		  : config.port == GPIO::F ? target::SYSCFG_EXTI::PortF
	// 		  : config.port == GPIO::G ? target::SYSCFG_EXTI::PortG
	// 		  : config.port == GPIO::H ? target::SYSCFG_EXTI::PortH
	// 		  : config.port == GPIO::I ? target::SYSCFG_EXTI::PortI
	// 		  : config.port == GPIO::J ? target::SYSCFG_EXTI::PortJ
	// 								   : target::SYSCFG_EXTI::PortK;
	// if (config.pin == 1)
	// 	target::SYSCFG_EXTI::Pin1::write(port);
	// if (config.pin == 2)
	// 	target::SYSCFG_EXTI::Pin2::write(port);
	// if (config.pin == 3)
	// 	target::SYSCFG_EXTI::Pin3::write(port);
	// if (config.pin == 4)
	// 	target::SYSCFG_EXTI::Pin4::write(port);
	// if (config.pin == 5)
	// 	target::SYSCFG_EXTI::Pin5::write(port);
	// if (config.pin == 6)
	// 	target::SYSCFG_EXTI::Pin6::write(port);
	// if (config.pin == 7)
	// 	target::SYSCFG_EXTI::Pin7::write(port);
	// if (config.pin == 8)
	// 	target::SYSCFG_EXTI::Pin8::write(port);
	// if (config.pin == 9)
	// 	target::SYSCFG_EXTI::Pin9::write(port);
	// if (config.pin == 10)
	// 	target::SYSCFG_EXTI::Pin10::write(port);
	// if (config.pin == 11)
	// 	target::SYSCFG_EXTI::Pin11::write(port);
	// if (config.pin == 12)
	// 	target::SYSCFG_EXTI::Pin12::write(port);
	// if (config.pin == 13)
	// 	target::SYSCFG_EXTI::Pin13::write(port);
	// if (config.pin == 14)
	// 	target::SYSCFG_EXTI::Pin14::write(port);
	// if (config.pin == 15)
	// 	target::SYSCFG_EXTI::Pin15::write(port);

	// // Todo: use set/clear, so we need target::EXTI_::RTSR_Pin<4>
	// if (config.on_rising_edge)
	// 	target::EXTI_RTSR1::write(1 << _pin);
	// else {
	// 	auto tmp = target::EXTI_RTSR1::read() & ~(1 << _pin);
	// 	target::EXTI_RTSR1::write(tmp);
	// }

	// if (config.on_falling_edge)
	// 	target::EXTI_FTSR1::write(1 << _pin);
	// else {
	// 	auto tmp = target::EXTI_FTSR1::read() & ~(1 << _pin);
	// 	target::EXTI_FTSR1::write(tmp);
	// }

	// auto irqn = config.pin >= 10 ? EXTI15_10_IRQn
	// 		  : config.pin >= 5	 ? EXTI9_5_IRQn
	// 		  : config.pin == 0	 ? EXTI0_IRQn
	// 		  : config.pin == 1	 ? EXTI1_IRQn
	// 		  : config.pin == 2	 ? EXTI2_IRQn
	// 		  : config.pin == 3	 ? EXTI3_IRQn
	// 							 : EXTI4_IRQn;
	// InterruptManager::registerISR(irqn, config.priority1, config.priority2, [&]() {
	// 	if (target::EXTI_PR1::read() == (1 << _pin)) {
	// 		target::EXTI_PR1::write(1 << _pin);
	// 		task_func();
	// 	}
	// });
}

void PinChangeInterrupt::start() {
	// target::EXTI_IMR1::write(1 << _pin);
}
void PinChangeInterrupt::stop() {
	// auto tmp = target::EXTI_IMR1::read() & ~(1 << _pin);
	// target::EXTI_IMR1::write(tmp);
}
