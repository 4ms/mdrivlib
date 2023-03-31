#pragma once
#include "drivers/exti.hh"
#include "drivers/interrupt.hh"
#include "drivers/pin.hh"
#include "drivers/pin_change_conf.hh"
#include "drivers/rcc.hh"
#include <functional>

namespace mdrivlib
{

//Note: this is a lot of boilerplate between f0, f4, f7 and h7.
// All the changes between chips is the list of GPIOs
// and the selection of IRQn from pin number
// We could probably just define get_irqn_from_pinnum() and get_extiport_from_gpioport()
// And have the rest be common to all
template<PinChangeConf ConfT>
class PinChangeInt {
public:
	PinChangeInt() = default;

	PinChangeInt(std::function<void(void)> &&func)
		: task_func{std::move(func)} {
		_init();
		_init_irq();
	}

	void init(std::function<void(void)> &&func) {
		task_func = std::move(func);
		_init();
		_init_irq();
	}

	// Useful for using your own IRQ handler
	// Must enable the IRQ with NVIC/GIC yourself, and call PinChangeInt::start()
	// Also must read and set the PinChangePending flag in your IRQ handler
	// Note: on an F030 with 48MHz SysClk, using lambdas and InterruptManager added
	// ~100ns (5-6 instructions) to the response time.
	void init_without_handler() {
		_init();
	}

	void start() {
		EXTI_::PinInterruptMask<ConfT::pin>::set();
	}

	void stop() {
		EXTI_::PinInterruptMask<ConfT::pin>::clear();
	}

private:
	void _init() {
		// RCC_Enable::SYSCFGCOMP_::set();
		// auto port = EXTI_::GetPort<ConfT::port>();
		auto port = ConfT::port == GPIO::A ? EXTI_::PortA
				  : ConfT::port == GPIO::B ? EXTI_::PortB
				  : ConfT::port == GPIO::C ? EXTI_::PortC
				  : ConfT::port == GPIO::D ? EXTI_::PortD
				  : ConfT::port == GPIO::E ? EXTI_::PortE
				  : ConfT::port == GPIO::F ? EXTI_::PortF
				  : ConfT::port == GPIO::G ? EXTI_::PortG
				  : ConfT::port == GPIO::H ? EXTI_::PortH
										   // : ConfT::port == GPIO::J ? EXTI_::PortJ
										   // : ConfT::port == GPIO::K ? EXTI_::PortK
										   : EXTI_::PortA;
		if constexpr (ConfT::pin == 1)
			EXTI_::Pin1::write(port);
		if constexpr (ConfT::pin == 2)
			EXTI_::Pin2::write(port);
		if constexpr (ConfT::pin == 3)
			EXTI_::Pin3::write(port);
		if constexpr (ConfT::pin == 4)
			EXTI_::Pin4::write(port);
		if constexpr (ConfT::pin == 5)
			EXTI_::Pin5::write(port);
		if constexpr (ConfT::pin == 6)
			EXTI_::Pin6::write(port);
		if constexpr (ConfT::pin == 7)
			EXTI_::Pin7::write(port);
		if constexpr (ConfT::pin == 8)
			EXTI_::Pin8::write(port);
		if constexpr (ConfT::pin == 9)
			EXTI_::Pin9::write(port);
		if constexpr (ConfT::pin == 10)
			EXTI_::Pin10::write(port);
		if constexpr (ConfT::pin == 11)
			EXTI_::Pin11::write(port);
		if constexpr (ConfT::pin == 12)
			EXTI_::Pin12::write(port);
		if constexpr (ConfT::pin == 13)
			EXTI_::Pin13::write(port);
		if constexpr (ConfT::pin == 14)
			EXTI_::Pin14::write(port);
		if constexpr (ConfT::pin == 15)
			EXTI_::Pin15::write(port);

		if constexpr (ConfT::on_rising_edge)
			EXTI_::PinRisingTrig<ConfT::pin>::set();
		else
			EXTI_::PinRisingTrig<ConfT::pin>::clear();

		if constexpr (ConfT::on_falling_edge)
			EXTI_::PinFallingTrig<ConfT::pin>::set();
		else
			EXTI_::PinFallingTrig<ConfT::pin>::clear();
	}

	void _init_irq() {
		// auto irqn = EXTI_::GetIRQn<ConfT::pin>();
		auto irqn = ConfT::pin >= 10 ? EXTI15_10_IRQn
				  : ConfT::pin >= 5	 ? EXTI9_5_IRQn
				  : ConfT::pin == 4	 ? EXTI4_IRQn
				  : ConfT::pin == 3	 ? EXTI3_IRQn
				  : ConfT::pin == 2	 ? EXTI2_IRQn
				  : ConfT::pin == 1	 ? EXTI1_IRQn
									 : EXTI0_IRQn;

		InterruptManager::register_and_start_isr(irqn, ConfT::priority1, ConfT::priority2, [&]() {
			if (EXTI_::PinTrigPending<ConfT::pin>::read()) {
				EXTI_::PinTrigPending<ConfT::pin>::set(); // clear on write
				task_func();
			}
		});
	}

private:
	std::function<void(void)> task_func;
};
} // namespace mdrivlib
