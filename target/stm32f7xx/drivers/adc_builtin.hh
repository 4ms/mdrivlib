#pragma once
#include "drivers/adc_builtin_conf.hh"
#include "drivers/adc_common_builtin.hh"
#include "drivers/clocks.hh"
#include "drivers/dma_transfer.hh"
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/pin.hh"
#include "drivers/stm32xx.h"
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

	// FIXME: Common ADC IRQ gets called constantly
	void register_callback(auto &adc_common, Callback &&callback) {
		adc_common.register_callback(ConfT::adc_periph_num, std::move(callback));
		__HAL_ADC_DISABLE(&hadc);
		__HAL_ADC_ENABLE_IT(&hadc, ADC_IT_EOC);
		__HAL_ADC_ENABLE(&hadc);
	}

	void register_callback(auto callback) {
		dma.register_callback(std::move(callback));
	}

	template<AdcPeriphNum p>
	static constexpr ADC_TypeDef *get_ADC_base() {
		if constexpr (p == AdcPeriphNum::_1)
			return ADC1;
		else if (p == AdcPeriphNum::_2)
			return ADC2;
		else
			return ADC3;
	}

	ADC_HandleTypeDef hadc;
	DMATransfer<DmaConf> dma;

	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
