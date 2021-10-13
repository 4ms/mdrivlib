#include "clocks.hh"
#include "interrupt_control.hh"
#include "pin.hh"
#include "stm32mp1xx_hal_adc.h"
#include "stm32xx.h"
#include <array>

namespace mdrivlib
{

enum class AdcPeriphNum { _1, _2 };
enum class AdcChanNum { _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15 };
enum AdcSamplingTime { _1Cycle = 0b000, _2Cycles, _8Cycles, _16Cycles, _32Cycles, _64Cycles, _128Cycles, __810Cycles };

struct AdcPeriphConf {
	static constexpr AdcPeriphNum adc_periph_num = AdcPeriphNum::_1;

	// Data type
	using ValueT = uint16_t;
	// enum DataSize { Byte, HalfWord, Word };
	// static constexpr DataSize data_size = HalfWord;

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
	PinNoInit pin;
	AdcChanNum adc_chan_num;
	uint32_t rank = 0;
	AdcSamplingTime sampling_time = AdcSamplingTime::_32Cycles;
	// TODO:? bool auto_set_rank = true;
	// TODO: Single/diff
	// TODO: offset
};

template<typename ConfT> // requires derives from AdcPeriphConf
class AdcDmaPeriph {
	using ValueT = typename ConfT::ValueT;

public:
	template<size_t N>
	AdcDmaPeriph(std::array<ValueT, N> &dma_buffer, std::array<AdcChannelConf, N> ChanConfs)
		: _dma_buffer{dma_buffer.data()}
		, num_channels{N} {

		// for (auto chan : ChanConfs)
		// 	Pin init_adc_pin{chan.pin.gpio, chan.pin.pin, PinMode::Analog};
		Pin pot2{GPIO::C, 3, PinMode::Analog};
		Pin pot3{GPIO::A, 3, PinMode::Analog};

		Clocks::ADC::enable(get_ADC_base(ConfT::adc_periph_num));
		hadc.Init.NbrOfConversion = num_channels;

		HAL_ADC_Init(&hadc);

		hdma_adc1.Instance = DMA2_Stream7;		   // ConfT
		hdma_adc1.Init.Request = DMA_REQUEST_ADC1; // ConfT
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // ConfT
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // ConfT
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;		// ConfT
		hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // ConfT
		HAL_DMA_Init(&hdma_adc1);
		__HAL_LINKDMA(&hadc, DMA_Handle, hdma_adc1);

		ADC_MultiModeTypeDef multimode = {.Mode = ADC_MODE_INDEPENDENT};
		HAL_ADCEx_MultiModeConfigChannel(&hadc, &multimode);

		// for (auto chan : ChanConfs) {
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
		ADC_ChannelConfTypeDef pot_3_conf = {
			.Channel = ADC_CHANNEL_15,
			.Rank = ADC_REGULAR_RANK_2,
			.SamplingTime = ADC_SAMPLETIME_810CYCLES_5,
			.SingleDiff = ADC_SINGLE_ENDED,
			.OffsetNumber = ADC_OFFSET_NONE,
			.Offset = 0,
			.OffsetRightShift = DISABLE,
			.OffsetSignedSaturation = DISABLE,
		};
		HAL_ADC_ConfigChannel(&hadc, &pot_2_conf);
		HAL_ADC_ConfigChannel(&hadc, &pot_3_conf);
		// }
	}

	static constexpr ADC_TypeDef *get_ADC_base(AdcPeriphNum p) {
		return (p == AdcPeriphNum::_1) ? ADC1 : (p == AdcPeriphNum::_2) ? ADC2 : nullptr;
	}

	ADC_HandleTypeDef hadc = {
		.Instance = ADC1,
		.Init =
			{
				.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2,
				.Resolution = ADC_RESOLUTION_16B,
				.ScanConvMode = ADC_SCAN_ENABLE,
				.EOCSelection = ADC_EOC_SEQ_CONV,
				.LowPowerAutoWait = DISABLE,
				.ContinuousConvMode = ENABLE,
				.NbrOfConversion = 2,
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
	DMA_HandleTypeDef hdma_adc1;

	void start() {
		HAL_ADC_Start_DMA(&hadc, (uint32_t *)_dma_buffer, num_channels);
	}

	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
