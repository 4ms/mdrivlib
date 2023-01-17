#pragma once
#include "adc_builtin_conf.hh"
#include "clocks.hh"
#include "debug.hh"
#include "dma_transfer.hh"
#include "interrupt.hh"
#include "interrupt_control.hh"
#include "pin.hh"
#include "stm32xx.h"
#include <array>

namespace mdrivlib
{

template<AdcPeriphConf ConfT>
class AdcDmaPeriph {
	using ValueT = typename ConfT::ValueT;
	using DmaConf = typename ConfT::DmaConf;

public:
	template<size_t N>
	AdcDmaPeriph(std::array<ValueT, N> &dma_buffer, std::array<AdcChannelConf, N> ChanConfs)
		: _dma_buffer{dma_buffer.data()}
		, num_channels{N} {

		Clocks::ADCn<ConfT::adc_periph_num>::enable();
		hadc = {
			.Instance = get_ADC_base<ConfT::adc_periph_num>(),
			.Init =
				{
					.ClockPrescaler = ConfT::clock_div,
					.Resolution = ConfT::resolution,
					.DataAlign = ConfT::align,
					.ScanConvMode = ADC_SCAN_ENABLE,
					.EOCSelection = ADC_EOC_SEQ_CONV,
					.ContinuousConvMode = ENABLE,
					.NbrOfConversion = num_channels,
					.DiscontinuousConvMode = DISABLE,
					.NbrOfDiscConversion = 0,
					.ExternalTrigConv = ADC_SOFTWARE_START,
					.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
					.DMAContinuousRequests = ENABLE,
				},
		};
		HAL_ADC_Init(&hadc);

		dma.link_periph_to_dma(hadc);

		for (const auto &chan : ChanConfs) {
			Pin init_adc_pin{chan.pin.gpio, chan.pin.pin, PinMode::Analog};

			ADC_ChannelConfTypeDef adc_chan_conf = {
				.Channel = static_cast<uint32_t>(chan.adc_chan_num),
				.Rank = chan.rank + 1,
				.SamplingTime = chan.sampling_time,
			};
			HAL_ADC_ConfigChannel(&hadc, &adc_chan_conf);
		}
	}

	void start() {
		HAL_ADC_Start_DMA(&hadc, (uint32_t *)_dma_buffer, num_channels);
	}

	ValueT get_val(unsigned chan) {
		return _dma_buffer[chan];
	}

	void register_callback(auto callback, uint32_t pri, uint32_t subpri) {
		__HAL_ADC_DISABLE(&hadc);
		__HAL_ADC_ENABLE_IT(&hadc, ADC_IT_EOC);
		__HAL_ADC_ENABLE(&hadc);

		//TODO: seems to work without clearing the EOC flag, but why?
		// Interrupt::register_and_start_isr(ADC_IRQn, pri, subpri, [callback = std::move(callback), this] {
		// 	__HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_EOC);
		// 	callback();
		// });
		Interrupt::register_and_start_isr(ADC_IRQn, pri, subpri, callback);
	}

	template<AdcPeriphNum p>
	static constexpr ADC_TypeDef *get_ADC_base() {
		if constexpr (p == AdcPeriphNum::_1)
			return ADC1;
		else
			return ADC2;
	}

	ADC_HandleTypeDef hadc;
	DMATransfer<DmaConf> dma;

	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
