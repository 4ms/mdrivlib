#pragma once
#include "adc_builtin_conf.hh"
#include "clocks.hh"
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

		//On the F0 series, sampling time is common to all channels:
		uint32_t common_sampling_time = ChanConfs[0].sampling_time;

		Clocks::ADC::enable(get_ADC_base(ConfT::adc_periph_num));
		hadc = {
			.Instance = get_ADC_base(ConfT::adc_periph_num),
			.Init =
				{
					.ClockPrescaler = ConfT::clock_div,
					.Resolution = ConfT::resolution,
					.DataAlign = ConfT::align,
					.ScanConvMode = ADC_SCAN_ENABLE,
					.EOCSelection = ADC_EOC_SEQ_CONV,
					.LowPowerAutoWait = DISABLE,
					.LowPowerAutoPowerOff = DISABLE,
					.ContinuousConvMode = ENABLE,
					.DiscontinuousConvMode = DISABLE,
					.ExternalTrigConv = ADC_SOFTWARE_START,
					.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
					.DMAContinuousRequests = ENABLE,
					.Overrun = ADC_OVR_DATA_OVERWRITTEN,
					.SamplingTimeCommon = common_sampling_time,
				},
		};
		HAL_ADC_Init(&hadc);

		dma.link_periph_to_dma(hadc);

		for (const auto &chan : ChanConfs) {
			Pin init_adc_pin{chan.pin.gpio, chan.pin.pin, PinMode::Analog};

			ADC_ChannelConfTypeDef adc_chan_conf = {
				.Channel = chan.adc_chan_num,
				.Rank = chan.rank,
				.SamplingTime = common_sampling_time,
			};
			HAL_ADC_ConfigChannel(&hadc, &adc_chan_conf);
		}
	}

	void start() {
		HAL_ADC_Start_DMA(&hadc, (uint32_t *)_dma_buffer, num_channels);

		uint32_t reg = hadc.Instance->IER;

		// if constexpr (ConfT::enable_end_of_sequence_isr)
		// 	reg |= ADC_IER_EOSIE;
		// else
		reg &= ~ADC_IER_EOSIE;

		// if constexpr (ConfT::enable_end_of_conversion_isr)
		// 	reg |= ADC_IER_EOCIE;
		// else
		reg &= ~ADC_IER_EOCIE;

		reg &= ~ADC_IER_OVRIE;	 //overrun
		reg &= ~ADC_IER_EOSMPIE; //end of sampling phase of any channel
		reg &= ~ADC_IER_ADRDYIE; //adc ready after being turned on

		hadc.Instance->IER = reg;
	}

	static constexpr ADC_TypeDef *get_ADC_base(AdcPeriphNum p) {
		return ADC1;
	}

	ADC_HandleTypeDef hadc;
	DMATransfer<DmaConf> dma;

	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
