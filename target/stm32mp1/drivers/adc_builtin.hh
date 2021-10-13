#include "clocks.hh"
#include "interrupt_control.hh"
#include "stm32mp1xx_hal_adc.h"
#include "stm32xx.h"

namespace mdrivlib
{

enum class AdcPeriphNum { _1, _2 };

struct AdcPeriphConf {
	static constexpr AdcPeriphNum adc_periph_num = AdcPeriphNum::_1;

	// Data type
	enum DataSize { Byte, HalfWord, Word };
	static constexpr DataSize data_size = HalfWord;

	enum Resolution { Bits8, Bits10, Bits12, Bits14, Bits16 };
	static constexpr Resolution resolution = Bits16;

	// Ovesampling
	static constexpr bool oversample = false;
	static constexpr uint32_t oversampling_ratio = 0;
	static constexpr uint32_t oversampling_right_bitshift = 0;
	// TODO: other os config opts?

	// DMA
	static constexpr bool use_dma = true;
	enum DMAPeriph { DMA_1, DMA_2 };
	static constexpr DMAPeriph dma_periph_num = DMA_1;
	static constexpr uint32_t stream_num = 0;
	static constexpr uint32_t request_num = 0;
	enum Priority { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };
	static constexpr uint8_t dma_priority = Low;

	static constexpr bool use_dma_fifo = false;
	// TODO: threshold

	// DMA IRQ
	static constexpr bool use_dma_irq = false;
	static constexpr IRQn_Type dma_IRQn = (IRQn_Type)0;
	static constexpr uint32_t pri = 0;
	static constexpr uint32_t subpri = 0;
};

struct AdcChannelConf {
	using ADCx = AdcPeriphConf;
	static constexpr uint32_t adc_chan_num = 0;

	enum SamplingTime { _1Cycle = 0b000, _2Cycles, _8Cycles, _16Cycles, _32Cycles, _64Cycles, _128Cycles, __810Cycles };
	static constexpr SamplingTime sampling_time = _32Cycles;

	static constexpr bool auto_set_rank = true;
	// static constexpr uint32_t rank = 1;
	// TODO: manually set rank
	// TODO: Single/diff
	// TODO: offset
};

template<AdcPeriphNum PeriphNum>
class AdcPeriph {
	AdcPeriph() {
		Clocks::ADC::enable(ADCx);
		HAL_ADC_Init(&hadc);
		hdma_adc1.Instance = DMA2_Stream7;
		hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
		hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma_adc1);
		__HAL_LINKDMA(&hadc, DMA_Handle, hdma_adc1);

		ADC_MultiModeTypeDef multimode = {.Mode = ADC_MODE_INDEPENDENT};
		HAL_ADCEx_MultiModeConfigChannel(&hadc, &multimode);
	}

	AdcPeriph<PeriphNum> &AdcInstance() {
		static AdcPeriph<PeriphNum> Adc_;
		return Adc_;
	}

	static constexpr ADC_TypeDef *get_ADC_base(AdcPeriphNum p) {
		return (p == AdcPeriphNum::_1) ? ADC1 : (p == AdcPeriphNum::_2) ? ADC2 : nullptr;
	}
	static inline ADC_TypeDef *ADCx = get_ADC_base(PeriphNum);

	static inline ADC_HandleTypeDef hadc = {
		.Instance = ADC1,
		.Init =
			{
				.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2,
				.Resolution = ADC_RESOLUTION_16B,
				.ScanConvMode = ADC_SCAN_ENABLE,
				.EOCSelection = ADC_EOC_SEQ_CONV,
				.LowPowerAutoWait = DISABLE,
				.ContinuousConvMode = ENABLE,
				.NbrOfConversion = 0,
				.DiscontinuousConvMode = DISABLE,
				.NbrOfDiscConversion = 0,
				.ExternalTrigConv = ADC_SOFTWARE_START,
				.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
				.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR,
				.Overrun = ADC_OVR_DATA_OVERWRITTEN,
				.LeftBitShift = ADC_LEFTBITSHIFT_NONE,
				.OversamplingMode = ENABLE,
				.Oversampling =
					{
						.Ratio = 1024,
						.RightBitShift = ADC_RIGHTBITSHIFT_10,
						.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER,
						.OversamplingStopReset = ADC_REGOVERSAMPLING_RESUMED_MODE,
					},
			},
	};
	static inline DMA_HandleTypeDef hdma_adc1;

	template<typename ChannelConf>
	void add_channel() {
		//
	}

	void start() {
		HAL_ADC_Start_DMA(&hadc, (uint32_t *)values, 2);
	}

	uint32_t values[16];
};

// Todo concept requires for ConfT
template<typename ConfT>
class AdcChannel {
	//
	AdcChannel() {
		auto init_adc_once = AdcPeriph<ConfT::adc_periph_num>::AdcInstance();
		// AdcPeriph<ADCN>::add_channel(c, sampletime);
		// add_channel
		ADC_ChannelConfTypeDef pot_2_conf = {
			.Channel = ADC_CHANNEL_13,
			.Rank = ADC_REGULAR_RANK_1,
			.SamplingTime = ADC_SAMPLETIME_810CYCLES_5,
			.SingleDiff = ADC_SINGLE_ENDED,
			.OffsetNumber = ADC_OFFSET_NONE,
			.Offset = 0,
			.OffsetRightShift = DISABLE,
			.OffsetSignedSaturation = DISABLE,
		};
		HAL_ADC_ConfigChannel(&AdcPeriph<ConfT::adc_periph_num>::hadc, &pot_2_conf);
	}
};

} // namespace mdrivlib
