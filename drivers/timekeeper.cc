#include "drivers/timekeeper.hh"
#include "drivers/arch.hh"
#include "drivers/clocks.hh"
#include "drivers/interrupt.hh"
#include "drivers/stm32xx.h"
#include "drivers/tim.hh"

namespace mdrivlib
{

void Timekeeper::init(const TimekeeperConfig &config, CallbackT &&func) {
	_set_periph(config.TIMx);
	task_func = std::move(func);
	_init(config);
}

Timekeeper::Timekeeper(const TimekeeperConfig &config, CallbackT &&func)
	: timx(config.TIMx)
	, irqn(PeriphUtil::TIM::IRQn(timx))
	, task_func(std::move(func)) {
	_init(config);
}

void Timekeeper::_init(const TimekeeperConfig &config) {
	_register_task();
	Clocks::TIM::enable(timx);
	_set_timing(config.period_ns, config.priority1, config.priority2);
}

void Timekeeper::start() {
#ifdef STM32MP1
	printf("Starting timer irq %u LevelTriggered\n", irqn);
	InterruptControl::enable_irq(irqn, InterruptControl::LevelTriggered);
#else
	InterruptControl::enable_irq(irqn);
#endif
	tim_update_IT_clear();
}

// Similar to start(), but on MP1 A7 , it does
// not change the core the interrupt runs on
void Timekeeper::unpause() {
	InterruptControl::reenable_irq(irqn);
	tim_update_IT_clear();
}

void Timekeeper::stop() {
	InterruptControl::disable_irq(irqn);
}

void Timekeeper::_set_periph(TIM_TypeDef *TIMx) {
	timx = TIMx;
	irqn = PeriphUtil::TIM::IRQn(timx);
}

// Todo: Test this!
void Timekeeper::_set_timing(uint32_t period_ns, uint32_t priority1, uint32_t priority2) {
	const uint32_t periph_num = PeriphUtil::TIM::to_num(timx);
	const uint32_t max_period = PeriphUtil::TIM::max_period(periph_num);
	const uint32_t max_prescaler = PeriphUtil::TIM::max_prescaler(periph_num);
	const uint32_t max_clockdivider = PeriphUtil::TIM::max_clockdivider(periph_num);
	const uint32_t sysfreq_Hz = PeriphUtil::TIM::max_freq(timx);
	const float sysfreq_ns = 1'000'000'000.f / sysfreq_Hz;

	uint32_t period_clocks = period_ns / sysfreq_ns;

	uint32_t prescaler = 0;
	while ((period_clocks / (prescaler + 1)) > max_period) {
		prescaler++;
	}
	period_clocks = period_clocks / (prescaler + 1);

	uint32_t clock_division = 0;
	while ((prescaler / (clock_division + 1) > max_prescaler)) {
		auto next_clock_division = PeriphUtil::TIM::next_allowed_clockdivision(periph_num, clock_division);
		if (next_clock_division == clock_division)
			break;
		clock_division = next_clock_division;
	}
	prescaler = prescaler / (clock_division + 1);

	if (prescaler > max_prescaler) {
		prescaler = max_prescaler;
	}
	if (clock_division > max_clockdivider) {
		period_clocks = max_period;
		prescaler = max_prescaler;
		clock_division = max_clockdivider;
	}

	InterruptControl::set_irq_priority(irqn, priority1, priority2);

	TIMPeriph::init_periph(timx, period_clocks, prescaler, clock_division);

	LL_TIM_EnableIT_UPDATE(timx);
	LL_TIM_EnableCounter(timx);
}

void Timekeeper::_register_task() {
	InterruptManager::register_isr(irqn, [this]() {
		if (tim_update_IT_is_set()) {
			if (tim_update_IT_is_source()) {
				if (task_func)
					task_func();
			}
			tim_update_IT_clear();
		}
	});
}

bool Timekeeper::tim_update_IT_is_set() const {
	return LL_TIM_IsActiveFlag_UPDATE(timx);
}
bool Timekeeper::tim_update_IT_is_source() const {
	return LL_TIM_IsEnabledIT_UPDATE(timx);
}
void Timekeeper::tim_update_IT_clear() const {
	LL_TIM_ClearFlag_UPDATE(timx);
}
} // namespace mdrivlib

// Todo: allows doubling-up on the IRQs that have two TIM UP on a single IRQ.
// e.g.: TIM1/TIM10, and TIM8/TIM13 share an ISR
// Maybe just need a special-case in register_task:
// if (IRQn == TIM1_UP_TIM10_IRQn) {
// ...inside lamba:
// if (tim_update_IT_is_set(TIM1)) {...
// else if (tim_update_IT_is_set(TIM10)) {...
