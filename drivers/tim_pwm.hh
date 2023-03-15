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

// TimPwmChannel: output PWM on a pin.
// Automatically initializes the given TIM peripheral if it hasn't been
// initialized yet
// Usage: 		Pin {GPIO::C, 2, LL_GPIO_AF_1};
//      TimPwmChannel myPWM {TIM1, TimChannelNum::_3}; //default period is 256
//      myPWM.set(128); // output starts immediately
//      myPWM.stop_output();
//
// Todo: add separate methods for setting pin, TIM, channel, period, prescaler, clock_div

struct TimChanConf {
	PinDef pin;
	TIM_TypeDef *const TIM;
	TimChannelNum channum;
	uint32_t period = 256;
	uint32_t prescaler = 1;
	uint32_t clock_div = 0;
};

template<TimChanConf Conf>
class TimPwmChan {

	TimPwmChan() {
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

	void set(uint32_t val) const {
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

	void start_output() const {
		LL_TIM_CC_EnableChannel(get_TIM(), static_cast<uint32_t>(Conf.channum));
	}
	void stop_output() const {
		LL_TIM_CC_DisableChannel(get_TIM(), static_cast<uint32_t>(Conf.channum));
	}

private:
	constexpr TimChannelNum get_base_channel() {
		if constexpr (Conf.channum == TimChannelNum::_1N)
			return TimChannelNum::_1;
		if constexpr (Conf.channum == TimChannelNum::_2N)
			return TimChannelNum::_2;
		if constexpr (Conf.channum == TimChannelNum::_3N)
			return TimChannelNum::_3;
		return Conf.channum;
	}

	constexpr bool is_inverted() {
		return get_base_channel() == Conf.channum;
	}

	constexpr TIM_TypeDef *get_TIM() {
		return Conf.TIM;
		//return reinterpret_cast<TIM_TypeDef*>(Conf.TIM);
	}
};

class TimPwmChannel {
public:
	TimPwmChannel(TIM_TypeDef *const TIM,
				  TimChannelNum const channel,
				  uint32_t period = 256,
				  uint16_t prescaler = 1,
				  uint32_t clock_division = 0)
		: TIM_(TIM)
		, channel_(channel) {
		TIMPeriph::init_periph(TIM, period, prescaler, clock_division);

		if (IS_TIM_BREAK_INSTANCE(TIM))
			LL_TIM_EnableAllOutputs(TIM);

		bool inverted_channel;
		if (channel_ == TimChannelNum::_1N) {
			channel_base_ = TimChannelNum::_1;
			inverted_channel = true;
		} else if (channel_ == TimChannelNum::_2N) {
			channel_base_ = TimChannelNum::_2;
			inverted_channel = true;
		} else if (channel_ == TimChannelNum::_3N) {
			channel_base_ = TimChannelNum::_3;
			inverted_channel = true;
		} else {
			channel_base_ = channel_;
			inverted_channel = false;
		}

		LL_TIM_OC_InitTypeDef tim_oc = {
			.OCMode = LL_TIM_OCMODE_PWM1,
			.OCState = inverted_channel ? LL_TIM_OCSTATE_DISABLE : LL_TIM_OCSTATE_ENABLE,
			.OCNState = inverted_channel ? LL_TIM_OCSTATE_ENABLE : LL_TIM_OCSTATE_DISABLE,
			.CompareValue = 0,
			.OCPolarity = LL_TIM_OCPOLARITY_LOW,
			.OCNPolarity = LL_TIM_OCPOLARITY_LOW,
			.OCIdleState = LL_TIM_OCIDLESTATE_LOW,
			.OCNIdleState = LL_TIM_OCIDLESTATE_LOW,
		};

		LL_TIM_OC_Init(TIM_, static_cast<uint32_t>(channel_base_), &tim_oc);
		LL_TIM_CC_EnableChannel(TIM_, static_cast<uint32_t>(channel_));
		LL_TIM_EnableCounter(TIM_);
	}

	constexpr void set(uint32_t val) const {
		_set_timer_ccr(TIM_, channel_base_, val);
	}
	void start_output() const {
		LL_TIM_CC_EnableChannel(TIM_, static_cast<uint32_t>(channel_));
	}
	void stop_output() const {
		LL_TIM_CC_DisableChannel(TIM_, static_cast<uint32_t>(channel_));
	}

protected:
	TimPwmChannel()
		: TIM_(nullptr) {
	}

private:
	constexpr void _set_timer_ccr(TIM_TypeDef *TIMx, TimChannelNum channel, uint32_t val) const {
		switch (channel) {
			case TimChannelNum::_1:
				TIMx->CCR1 = val;
				break;
			case TimChannelNum::_2:
				TIMx->CCR2 = val;
				break;
			case TimChannelNum::_3:
				TIMx->CCR3 = val;
				break;
			case TimChannelNum::_4:
				TIMx->CCR4 = val;
				break;
#ifdef LL_TIM_CHANNEL_CH5
			case TimChannelNum::_5:
				TIMx->CCR5 = val;
				break;
#endif
#ifdef LL_TIM_CHANNEL_CH6
			case TimChannelNum::_6:
				TIMx->CCR1 = val;
				break;
#endif
			default:
				TIMx->CCR1 = val;
				break;
		}
	}

	TIM_TypeDef *const TIM_;
	TimChannelNum channel_;
	TimChannelNum channel_base_;
};

// Dummy class for use when an LED element of an RGB LED is not connected
class NoPwmChannel : public TimPwmChannel {
public:
	NoPwmChannel() {
	}
	void set(uint32_t val) const {
	}
};
} // namespace mdrivlib
