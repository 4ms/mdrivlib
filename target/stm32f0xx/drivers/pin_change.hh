#pragma once
#include "exti.hh"
#include "interrupt.hh"
#include "pin.hh"
#include "pin_change_conf.hh"
#include "rcc.hh"
#include <functional>

namespace mdrivlib
{

template<PinChangeConf ConfT>
class PinChangeInt {
public:
	PinChangeInt() = default;

	PinChangeInt(std::function<void(void)> &&func)
		: task_func{std::move(func)} {
		_init();
	}

	void init(std::function<void(void)> &&func) {
		task_func = std::move(func);
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
		RCC_Enable::SYSCFGCOMP_::set();
		auto port = ConfT::port == GPIO::A ? EXTI_::PortA
				  : ConfT::port == GPIO::B ? EXTI_::PortB
				  : ConfT::port == GPIO::C ? EXTI_::PortC
				  : ConfT::port == GPIO::D ? EXTI_::PortD
				  : ConfT::port == GPIO::F ? EXTI_::PortF
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

		auto irqn = ConfT::pin >= 4 ? EXTI4_15_IRQn : ConfT::pin >= 2 ? EXTI2_3_IRQn : EXTI0_1_IRQn;

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
