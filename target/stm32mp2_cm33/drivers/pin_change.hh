#pragma once
#include "drivers/interrupt.hh"
#include "drivers/pin.hh"
#include "drivers/pin_change_conf.hh"
#include "drivers/stm32xx.h"
#include <functional>

namespace mdrivlib
{

// Pin change interrupt for the STM32MP2's Cortex-M33 (CPU2), on EXTI1. Same API as the other
// targets' PinChangeInt.
// The EXTI sees a pin's input even when the pin is in an alternate function mode, e.g. an SAI frame
// clock the MCU drives.
template<PinChangeConf ConfT>
class PinChangeInt {
	using InterruptT = std::function<void()>;

	static_assert(ConfT::pin < 16, "EXTI1 lines 0-15 are the GPIO pins");
	static constexpr uint32_t bit = 1u << ConfT::pin;

public:
	PinChangeInt() = default;

	PinChangeInt(InterruptT &&func)
		: task_func{std::move(func)} {
		_init();
	}

	void init(InterruptT &&func) {
		task_func = std::move(func);
		_init();
	}

	void start() {
		EXTI1->C2IMR1 = EXTI1->C2IMR1 | bit;
	}

	void stop() {
		EXTI1->C2IMR1 = EXTI1->C2IMR1 & ~bit;
	}

private:
	// EXTICR's port selection (EXTI_GPIOx in the HAL)
	static constexpr uint32_t port_sel() {
		constexpr GPIO ports[] = {GPIO::A, GPIO::B, GPIO::C, GPIO::D, GPIO::E, GPIO::F,
								  GPIO::G, GPIO::H, GPIO::I, GPIO::J, GPIO::K, GPIO::Z};
		for (uint32_t i = 0; i < std::size(ports); i++) {
			if (ports[i] == ConfT::port)
				return i;
		}
		return 0;
	}

	void _init() {
		stop();

		constexpr auto shift = (ConfT::pin % 4) * EXTI_EXTICR1_EXTI1_Pos;
		auto &cr = EXTI1->EXTICR[ConfT::pin / 4];
		cr = (cr & ~(EXTI_EXTICR1_EXTI0_Msk << shift)) | (port_sel() << shift);

		if constexpr (ConfT::on_rising_edge)
			EXTI1->RTSR1 = EXTI1->RTSR1 | bit;
		else
			EXTI1->RTSR1 = EXTI1->RTSR1 & ~bit;

		if constexpr (ConfT::on_falling_edge)
			EXTI1->FTSR1 = EXTI1->FTSR1 | bit;
		else
			EXTI1->FTSR1 = EXTI1->FTSR1 & ~bit;

		// Clear on write
		EXTI1->RPR1 = bit;
		EXTI1->FPR1 = bit;

		auto irqn = static_cast<IRQn_Type>(EXTI1_0_IRQn + ConfT::pin);
		InterruptManager::register_and_start_isr(irqn, ConfT::priority1, ConfT::priority2, [this]() {
			if constexpr (ConfT::on_rising_edge) {
				if (EXTI1->RPR1 & bit) {
					EXTI1->RPR1 = bit;
					task_func();
				}
			}
			if constexpr (ConfT::on_falling_edge) {
				if (EXTI1->FPR1 & bit) {
					EXTI1->FPR1 = bit;
					task_func();
				}
			}
		});
	}

	InterruptT task_func;
};

} // namespace mdrivlib
