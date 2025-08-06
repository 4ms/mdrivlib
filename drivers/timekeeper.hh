#pragma once
#include "drivers/callable.hh"
#include "stm32xx.h"

namespace mdrivlib
{

struct TimekeeperConfig {
	TIM_TypeDef *TIMx;
	uint32_t period_ns;
	uint32_t priority1;
	uint32_t priority2;

	static constexpr uint32_t Hz(uint32_t freq_hz) {
		return 1'000'000'000UL / freq_hz;
	}
};

class Timekeeper {
	using CallbackT = CallbackSized<32>;

public:
	Timekeeper() = default;
	Timekeeper(const TimekeeperConfig &config, CallbackT &&func);
	void init(const TimekeeperConfig &config, CallbackT &&func);

	void start();
	void restart();
	void stop();
	void pause() const;
	void resume() const;

private:
	TIM_TypeDef *timx = nullptr;
	IRQn_Type irqn{};

	CallbackT task_func{};

	void _set_periph(TIM_TypeDef *timx);
	void _set_timing(uint32_t period_ns, uint32_t priority1 = 3, uint32_t priority2 = 3);
	void _register_task();
	void _init(const TimekeeperConfig &config);

	[[nodiscard]] bool tim_update_IT_is_set() const;
	[[nodiscard]] bool tim_update_IT_is_source() const;
	void tim_update_IT_clear() const;
};
} // namespace mdrivlib
