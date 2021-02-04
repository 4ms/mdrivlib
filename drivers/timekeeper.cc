#include "timekeeper.hh"
#include "interrupt.hh"
#include "stm32xx.h"
#include "tim.hh"

Timekeeper::Timekeeper()
	: is_running(false) {
}

void Timekeeper::init(const TimekeeperConfig &config, std::function<void(void)> &&func) {
	_set_periph(config.TIMx);
	task_func = std::move(func);
	_init(config);
}

Timekeeper::Timekeeper(const TimekeeperConfig &config, std::function<void(void)> &&func)
	: timx(config.TIMx)
	, irqn(target::peripherals::TIM::IRQn(timx))
	, task_func(std::move(func))
	, is_running(false) {
	_init(config);
}

void Timekeeper::_init(const TimekeeperConfig &config) {
	_register_task();
	Clocks::TIM::enable(timx);
	_set_timing(config.period_ns, config.priority1, config.priority2);
}

void Timekeeper::start() {
	is_running = true;
}

void Timekeeper::stop() {
	is_running = false;
}

void Timekeeper::_set_periph(TIM_TypeDef *TIMx) {
	timx = TIMx;
	irqn = target::peripherals::TIM::IRQn(timx);
}

// Todo: Support 32-bit timers. Right now it runs 32-bit timers in 16-bit mode
void Timekeeper::_set_timing(uint32_t period_ns, uint32_t priority1, uint32_t priority2) {
	uint32_t sysfreq_Hz = target::peripherals::TIM::max_freq(timx);
	float sysfreq_ns = 1000000000.f / sysfreq_Hz;
	uint32_t period_clocks = period_ns / sysfreq_ns;

	uint32_t prescaler = 0;
	while ((period_clocks / (prescaler + 1)) > 65535) {
		prescaler++;
	}
	period_clocks = period_clocks / (prescaler + 1);

	uint32_t clock_division = 0;
	while ((prescaler / (clock_division + 1) > 255)) {
		clock_division++;
	}
	prescaler = prescaler / (clock_division + 1);

	if (clock_division > 255) {
		period_clocks = 65535;
		prescaler = 255;
		clock_division = 255;
	}

	auto pri = System::encode_nvic_priority(priority1, priority2);
	NVIC_SetPriority(irqn, pri);
	NVIC_EnableIRQ(irqn);

	TIMPeriph::init_periph(timx, period_clocks, prescaler, clock_division);

	LL_TIM_EnableIT_UPDATE(timx);
	LL_TIM_EnableCounter(timx);
}

void Timekeeper::_register_task() {
	InterruptManager::registerISR(irqn, [this]() {
		if (tim_update_IT_is_set()) {
			if (tim_update_IT_is_source()) {
				// Todo: what is the cost of std::function operator bool
				if (is_running && task_func)
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

// Todo: allows doubling-up on the IRQs that have two TIM UP on a single IRQ.
// e.g.: TIM1/TIM10, and TIM8/TIM13 share an ISR
// Maybe just need a special-case in register_task:
// if (IRQn == TIM1_UP_TIM10_IRQn) {
// ...inside lamba:
// if (tim_update_IT_is_set(TIM1)) {...
// else if (tim_update_IT_is_set(TIM10)) {...
