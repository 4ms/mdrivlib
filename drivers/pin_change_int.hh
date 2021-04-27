#pragma once
#include "arch.hh"
#include "drivers/exti.hh"
#include "pin.hh"
#include <functional>

struct PinChangeConf {
	static constexpr uint32_t pin = 0;
	static constexpr GPIO port = GPIO::A;
	static constexpr bool on_rising_edge = false;
	static constexpr bool on_falling_edge = false;
	static constexpr uint32_t priority1 = 3;
	static constexpr uint32_t priority2 = 3;
};

template<typename ConfT>
class PinChangeInt {
public:
	PinChangeInt();
	PinChangeInt(std::function<void(void)> &&func);
	void init(std::function<void(void)> &&func) {
		if constexpr (ConfT::on_rising_edge)
			target::EXTI_::PinRisingTrig<ConfT::pin>::set();
		else
			target::EXTI_::PinRisingTrig<ConfT::pin>::clear();

		if constexpr (ConfT::on_falling_edge)
			target::EXTI_::PinFallingTrig<ConfT::pin>::set();
		else
			target::EXTI_::PinFallingTrig<ConfT::pin>::clear();
	}
	void start();
	void stop();

private:
	uint32_t _pin;
	void _init();
	std::function<void(void)> task_func;
};
