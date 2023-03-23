#pragma once
#include "pin.hh"
#include "stm32xx.h"
#include "tim.hh"

namespace mdrivlib
{

enum class TimChannelNum {
	_1 = LL_TIM_CHANNEL_CH1,
	_1N = LL_TIM_CHANNEL_CH1N,
	_2 = LL_TIM_CHANNEL_CH2,
	_2N = LL_TIM_CHANNEL_CH2N,
	_3 = LL_TIM_CHANNEL_CH3,
	_3N = LL_TIM_CHANNEL_CH3N,
	_4 = LL_TIM_CHANNEL_CH4,
#ifdef LL_TIM_CHANNEL_CH5
	_5 = LL_TIM_CHANNEL_CH5,
#endif
#ifdef LL_TIM_CHANNEL_CH6
	_6 = LL_TIM_CHANNEL_CH6
#endif
};

struct TimChanConf {
	PinDef pin;
	uint32_t TIM;
	TimChannelNum channum;
	uint32_t period = 256;
	uint32_t prescaler = 1;
	uint32_t clock_div = 0;
};

template<TimChanConf Conf>
struct TimPwmChan {

	TimPwmChan() {
		init();
	}
	static void init() {
		Pin{Conf.pin, PinMode::Alt};
		TIMPeriph::init_periph(get_TIM(), Conf.period, Conf.prescaler, Conf.clock_div);

		if (IS_TIM_BREAK_INSTANCE(get_TIM()))
			LL_TIM_EnableAllOutputs(get_TIM());

		LL_TIM_OC_InitTypeDef tim_oc = {
			.OCMode = LL_TIM_OCMODE_PWM1,
			.OCState = is_inverted() ? LL_TIM_OCSTATE_DISABLE : LL_TIM_OCSTATE_ENABLE,
			.OCNState = is_inverted() ? LL_TIM_OCSTATE_ENABLE : LL_TIM_OCSTATE_DISABLE,
			.CompareValue = 0,
			.OCPolarity = LL_TIM_OCPOLARITY_LOW,
			.OCNPolarity = LL_TIM_OCPOLARITY_LOW,
			.OCIdleState = LL_TIM_OCIDLESTATE_LOW,
			.OCNIdleState = LL_TIM_OCIDLESTATE_LOW,
		};

		LL_TIM_OC_Init(get_TIM(), static_cast<uint32_t>(get_base_channel()), &tim_oc);
		LL_TIM_CC_EnableChannel(get_TIM(), static_cast<uint32_t>(Conf.channum));
		LL_TIM_EnableCounter(get_TIM());
	}

	static void set(uint32_t val) {
		if constexpr (Conf.channum == TimChannelNum::_1)
			get_TIM()->CCR1 = val;
		if constexpr (Conf.channum == TimChannelNum::_2)
			get_TIM()->CCR2 = val;
		if constexpr (Conf.channum == TimChannelNum::_3)
			get_TIM()->CCR3 = val;
		if constexpr (Conf.channum == TimChannelNum::_4)
			get_TIM()->CCR4 = val;
#ifdef LL_TIM_CHANNEL_CH5
		if constexpr (Conf.channum == TimChannelNum::_5)
			get_TIM()->CCR5 = val;
#endif
#ifdef LL_TIM_CHANNEL_CH6
		if constexpr (Conf.channum == TimChannelNum::_6)
			get_TIM()->CCR1 = val;
#endif
	}

	static void start_output() {
		LL_TIM_CC_EnableChannel(get_TIM(), static_cast<uint32_t>(Conf.channum));
	}
	static void stop_output() {
		LL_TIM_CC_DisableChannel(get_TIM(), static_cast<uint32_t>(Conf.channum));
	}

private:
	static constexpr TimChannelNum get_base_channel() {
		if constexpr (Conf.channum == TimChannelNum::_1N)
			return TimChannelNum::_1;
		if constexpr (Conf.channum == TimChannelNum::_2N)
			return TimChannelNum::_2;
		if constexpr (Conf.channum == TimChannelNum::_3N)
			return TimChannelNum::_3;
		return Conf.channum;
	}

	static constexpr bool is_inverted() {
		return get_base_channel() == Conf.channum;
	}

	static constexpr TIM_TypeDef *get_TIM() {
		return reinterpret_cast<TIM_TypeDef *>(Conf.TIM);
	}
};

// Dummy class for use when an LED element of an RGB LED is not connected
struct NoPwmChannel {
	NoPwmChannel() {
	}
	static void set(uint32_t val) {
	}
};

} // namespace mdrivlib
