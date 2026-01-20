#pragma once
#include "drivers/adc_builtin_conf.hh"
#include "drivers/clocks.hh"
#include "drivers/dma_transfer.hh"
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

		uint32_t common_sampling_time = ChanConfs[0].sampling_time;

		Clocks::ADCn<ConfT::adc_periph_num>::enable();

		hadc = {
			.Instance = get_ADC_base(),
			.Init =
				{
					.ClockPrescaler = ConfT::clock_div,
					.Resolution = ConfT::resolution,
					.DataAlign = ConfT::align,
					.GainCompensation = 0,
					.ScanConvMode = ADC_SCAN_ENABLE,
					.EOCSelection = ADC_EOC_SEQ_CONV,
					.LowPowerAutoWait = DISABLE,
					.ContinuousConvMode = ENABLE,
					.NbrOfConversion = ChanConfs.size(),
					.DiscontinuousConvMode = DISABLE,
					.NbrOfDiscConversion = 0,
					.ExternalTrigConv = ADC_SOFTWARE_START,
					.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
					.DMAContinuousRequests = ENABLE,
					.Overrun = ADC_OVR_DATA_OVERWRITTEN,
					.OversamplingMode = ENABLE,
					.Oversampling =
						{
							.Ratio = ConfT::oversampling_ratio,
							.RightBitShift = ADC_RIGHTBITSHIFT_2,
							.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER,
							.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE,
						},
				},
		};
		HAL_ADC_Init(&hadc);

		ADC_MultiModeTypeDef multimode = {0};
		multimode.Mode = ADC_MODE_INDEPENDENT;
		HAL_ADCEx_MultiModeConfigChannel(&hadc, &multimode);

		dma.link_periph_to_dma(hadc);

		for (const auto &chan : ChanConfs) {
			Pin init_adc_pin{chan.pin.gpio, chan.pin.pin, PinMode::Analog};

			ADC_ChannelConfTypeDef adc_chan_conf = {
				.Channel = static_cast<uint32_t>(chan.adc_chan_num),
				.Rank = chan.rank,
				.SamplingTime = common_sampling_time,
				.SingleDiff = ADC_SINGLE_ENDED,
				.OffsetNumber = ADC_OFFSET_NONE,
				.Offset = 0,
			};
			HAL_ADC_ConfigChannel(&hadc, &adc_chan_conf);
		}
	}

	void start() {
		HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
		HAL_Delay(10);

		HAL_ADC_Start_DMA(&hadc, (uint32_t *)_dma_buffer, num_channels);

		// Must do this as fix for HAL, which unconditionally enables HT interrupt
		if constexpr (!DmaConf::half_transfer_interrupt_enable)
			dma.disable_ht();

		uint32_t reg = hadc.Instance->IER;
		if constexpr (ConfT::enable_end_of_sequence_isr)
			reg &= ~ADC_IER_EOSIE;
		else
			reg |= ADC_IER_EOSIE;

		if constexpr (ConfT::enable_end_of_conversion_isr)
			reg &= ~ADC_IER_EOCIE;
		else
			reg |= ADC_IER_EOCIE;

		// Disable overrun interrupt
		reg &= ~ADC_IER_OVRIE;

		hadc.Instance->IER = reg;
	}

	void register_callback(auto callback) {
		uint32_t reg = hadc.Instance->IER;
		if constexpr (ConfT::enable_end_of_sequence_isr)
			reg &= ~ADC_IER_EOSIE;
		else
			reg |= ADC_IER_EOSIE;

		if constexpr (ConfT::enable_end_of_conversion_isr)
			reg &= ~ADC_IER_EOCIE;
		else
			reg |= ADC_IER_EOCIE;

		// Disable overrun interrupt
		reg &= ~ADC_IER_OVRIE;

		hadc.Instance->IER = reg;
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
