#pragma once
#include "debug.hh"
#include "drivers/adc_builtin_conf.hh"
#include "drivers/callable.hh"
#include "drivers/interrupt.hh"
#include "drivers/stm32xx.h"
#include <concepts>

namespace mdrivlib
{

template<AdcCommonIsrConfC ConfT>
class AdcCommonIsr {
private:
	Callback callbackADC1;
	Callback callbackADC2;
	Callback callbackADC3;

public:
	void register_callback(AdcPeriphNum p, Callback &&callback) {
		if (p == AdcPeriphNum::_1)
			callbackADC1 = std::move(callback);
		else if (p == AdcPeriphNum::_2)
			callbackADC2 = std::move(callback);
		else if (p == AdcPeriphNum::_3)
			callbackADC3 = std::move(callback);
	}

	void start_common_isr() {
		Interrupt::register_and_start_isr(ADC_IRQn, ConfT::pri, ConfT::subpri, [this] {
			if constexpr (ConfT::adc1_enabled) {
				// ADC1->SR = ~ADC_FLAG_STRT;
				if ((ADC1->CR1 & ADC_IT_EOC) && (ADC1->SR & ADC_FLAG_EOC)) {
					callbackADC1();
					ADC1->SR = ~(ADC_FLAG_EOC | ADC_FLAG_OVR);
					// ADC1->SR = ~ADC_FLAG_STRT;
				}
				// if ((ADC1->CR1 & ADC_IT_OVR) && (ADC1->SR & ADC_FLAG_OVR))
				// 	ADC1->SR = ~ADC_FLAG_OVR;
			}

			if constexpr (ConfT::adc2_enabled) {
				// ADC2->SR = ~ADC_FLAG_STRT;
				if ((ADC2->CR1 & ADC_IT_EOC) && (ADC2->SR & ADC_FLAG_EOC)) {
					callbackADC2();
					ADC2->SR = ~(ADC_FLAG_EOC | ADC_FLAG_OVR);
					// ADC2->SR = ~ADC_FLAG_STRT;
				}
				// if ((ADC2->CR1 & ADC_IT_OVR) && (ADC2->SR & ADC_FLAG_OVR))
				// 	ADC2->SR = ~ADC_FLAG_OVR;
			}

			if constexpr (ConfT::adc3_enabled) {
				// ADC3->SR = ~ADC_FLAG_STRT;
				if ((ADC3->CR1 & ADC_IT_EOC) && (ADC3->SR & ADC_FLAG_EOC)) {
					callbackADC3();
					ADC3->SR = ~(ADC_FLAG_EOC | ADC_FLAG_OVR);
					// ADC3->SR = ~ADC_FLAG_STRT;
				}
				// if ((ADC3->CR1 & ADC_IT_OVR) && (ADC3->SR & ADC_FLAG_OVR))
				// 	ADC3->SR = ~ADC_FLAG_OVR;
			}
		});
	}
};

} // namespace mdrivlib
