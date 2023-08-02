#pragma once
#include "drivers/adc_builtin_conf.hh"
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

		//On the F0 series, sampling time is common to all channels:
		uint32_t common_sampling_time = ChanConfs[0].sampling_time;

		Clocks::ADCn<ConfT::adc_periph_num>::enable();
		hadc = {
			.Instance = get_ADC_base(),
			.Init =
				{
					.ClockPrescaler = ConfT::clock_div,
					.Resolution = ConfT::resolution,
					.DataAlign = ConfT::align,
					.ScanConvMode = ENABLE,
					.EOCSelection = ADC_EOC_SEQ_CONV,
					.ContinuousConvMode = ENABLE,
					.NbrOfConversion = ChanConfs.size(),
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
				.Rank = chan.rank,
				.SamplingTime = common_sampling_time,
			};
			HAL_ADC_ConfigChannel(&hadc, &adc_chan_conf);
		}
	}

	void start() {
		HAL_ADC_Start_DMA(&hadc, (uint32_t *)_dma_buffer, num_channels);
		// Must do this as fix for HAL, which unconditionally enables HT interrupt
		if constexpr (!DmaConf::half_transfer_interrupt_enable)
			dma.disable_ht();

		uint32_t reg = hadc.Instance->CR1;
		// Disable End of Conversion ISR
		reg &= ~ADC_CR1_EOCIE;
		reg &= ~ADC_CR1_OVRIE; //overrun
		hadc.Instance->CR1 = reg;
	}

	void register_callback(auto callback) {
		dma.register_callback(std::move(callback));
	}

	static constexpr ADC_TypeDef *get_ADC_base() {
		if constexpr (ConfT::adc_periph_num == AdcPeriphNum::_1)
			return ADC1;
		else if (ConfT::adc_periph_num == AdcPeriphNum::_2)
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
