#pragma once
#include "drivers/adc_builtin_conf.hh"
#include "drivers/clocks.hh"
#include "drivers/dma_transfer.hh"
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/pin.hh"
#include "drivers/stm32xx.h"
#include "util/math.hh"
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

		if constexpr (N == 0)
			return;

		Clocks::ADCn<ConfT::adc_periph_num>::enable();
		hadc = {
			.Instance = get_ADC_base(ConfT::adc_periph_num),
			.Init =
				{
					.ClockPrescaler = ConfT::clock_div,
					.Resolution = ConfT::resolution,
					.ScanConvMode = ADC_SCAN_ENABLE,
					.EOCSelection = ADC_EOC_SEQ_CONV,
					.LowPowerAutoWait = DISABLE,
					.ContinuousConvMode = ConfT::continuous_adc ? ENABLE : DISABLE,
					.NbrOfConversion = num_channels,
					.DiscontinuousConvMode = DISABLE,
					.NbrOfDiscConversion = 0,
					.ExternalTrigConv = ADC_SOFTWARE_START,
					.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
					.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR,
					.Overrun = ADC_OVR_DATA_OVERWRITTEN,
					.LeftBitShift = ADC_LEFTBITSHIFT_NONE,
					.OversamplingMode = ConfT::oversample ? ENABLE : DISABLE,
					.Oversampling =
						{
							.Ratio = std::clamp<uint32_t>(ConfT::oversampling_ratio, 1, 1024),
							.RightBitShift = ConfT::oversampling_right_bitshift,
							.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER,
							.OversamplingStopReset = ADC_REGOVERSAMPLING_RESUMED_MODE,
						},
				},
		};
		HAL_ADC_Init(&hadc);

		dma.link_periph_to_dma(hadc);

		ADC_MultiModeTypeDef multimode = {.Mode = ADC_MODE_INDEPENDENT};
		HAL_ADCEx_MultiModeConfigChannel(&hadc, &multimode);

		// Todo: allow auto rank.
		// Todo: what if we manually set rank but skip some numbers?
		for (auto chan : ChanConfs) {
			Pin init_adc_pin{chan.pin.gpio, chan.pin.pin, PinMode::Analog};

			ADC_ChannelConfTypeDef adc_chan_conf = {
				.Channel = static_cast<uint32_t>(chan.adc_chan_num),
				.Rank = adc_number_to_rank(chan.rank),
				.SamplingTime = chan.sampling_time,
				.SingleDiff = ADC_SINGLE_ENDED,	 // Todo: allow conf
				.OffsetNumber = ADC_OFFSET_NONE, // Todo: allow conf
				.Offset = 0,
				.OffsetRightShift = DISABLE,
				.OffsetSignedSaturation = DISABLE,
			};
			HAL_ADC_ConfigChannel(&hadc, &adc_chan_conf);
		}
	}

	void start() {
		HAL_ADC_Start_DMA(&hadc, (uint32_t *)_dma_buffer, num_channels);

		uint32_t reg = hadc.Instance->IER;

		if constexpr (ConfT::enable_end_of_sequence_isr)
			reg |= ADC_IER_EOSIE;
		else
			reg &= ~ADC_IER_EOSIE;

		if constexpr (ConfT::enable_end_of_conversion_isr)
			reg |= ADC_IER_EOCIE;
		else
			reg &= ~ADC_IER_EOCIE;

		reg &= ~ADC_IER_OVRIE;	 //overrun
		reg &= ~ADC_IER_EOSMPIE; //end of sampling phase of any channel
		reg &= ~ADC_IER_ADRDYIE; //adc ready after being turned on

		hadc.Instance->IER = reg;
	}

	void register_callback(auto callback) {
		// dma.register_callback(std::move(callback));
	}

	static constexpr ADC_TypeDef *get_ADC_base(AdcPeriphNum p) {
		return (p == AdcPeriphNum::_1) ? ADC1 : (p == AdcPeriphNum::_2) ? ADC2 : nullptr;
	}
	static constexpr uint32_t adc_number_to_rank(const uint32_t x) {
		return x == 0  ? ADC_REGULAR_RANK_1
			 : x == 1  ? ADC_REGULAR_RANK_2
			 : x == 2  ? ADC_REGULAR_RANK_3
			 : x == 3  ? ADC_REGULAR_RANK_4
			 : x == 4  ? ADC_REGULAR_RANK_5
			 : x == 5  ? ADC_REGULAR_RANK_6
			 : x == 6  ? ADC_REGULAR_RANK_7
			 : x == 7  ? ADC_REGULAR_RANK_8
			 : x == 8  ? ADC_REGULAR_RANK_9
			 : x == 9  ? ADC_REGULAR_RANK_10
			 : x == 10 ? ADC_REGULAR_RANK_11
			 : x == 11 ? ADC_REGULAR_RANK_12
			 : x == 12 ? ADC_REGULAR_RANK_13
			 : x == 13 ? ADC_REGULAR_RANK_14
			 : x == 14 ? ADC_REGULAR_RANK_15
			 : x == 15 ? ADC_REGULAR_RANK_16
					   : 0;
	}

	ADC_HandleTypeDef hadc;
	DMATransfer<DmaConf> dma;
	// DMA_HandleTypeDef hdma_adc1;

	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
