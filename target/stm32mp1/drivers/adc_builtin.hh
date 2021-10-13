#include "clocks.hh"
#include "interrupt_control.hh"
#include "pin.hh"
#include "stm32mp1xx_hal_adc.h"
#include "stm32xx.h"
#include "util/math.hh"
#include <array>

namespace mdrivlib
{

enum class AdcPeriphNum { _1, _2 };
enum AdcChanNum : uint32_t {
	_0 = ADC_CHANNEL_0,
	_1 = ADC_CHANNEL_1,
	_2 = ADC_CHANNEL_2,
	_3 = ADC_CHANNEL_3,
	_4 = ADC_CHANNEL_4,
	_5 = ADC_CHANNEL_5,
	_6 = ADC_CHANNEL_6,
	_7 = ADC_CHANNEL_7,
	_8 = ADC_CHANNEL_8,
	_9 = ADC_CHANNEL_9,
	_10 = ADC_CHANNEL_10,
	_11 = ADC_CHANNEL_11,
	_12 = ADC_CHANNEL_12,
	_13 = ADC_CHANNEL_13,
	_14 = ADC_CHANNEL_14,
	_15 = ADC_CHANNEL_15,
	_16 = ADC_CHANNEL_16,
	_17 = ADC_CHANNEL_17,
	_18 = ADC_CHANNEL_18,
	_19 = ADC_CHANNEL_19,
};
enum AdcSamplingTime { _1Cycle = 0b000, _2Cycles, _8Cycles, _16Cycles, _32Cycles, _64Cycles, _128Cycles, __810Cycles };
enum AdcResolution {
	Bits8 = ADC_RESOLUTION_8B,
	Bits10 = ADC_RESOLUTION_10B,
	Bits12 = ADC_RESOLUTION_12B,
	Bits14 = ADC_RESOLUTION_14B,
	Bits16 = ADC_RESOLUTION_16B
};
enum AdcOversampleRightBitShift : uint32_t {
	NoShift = ADC_RIGHTBITSHIFT_NONE,
	Shift1Right = ADC_RIGHTBITSHIFT_1,
	Shift2Right = ADC_RIGHTBITSHIFT_2,
	Shift3Right = ADC_RIGHTBITSHIFT_3,
	Shift4Right = ADC_RIGHTBITSHIFT_4,
	Shift5Right = ADC_RIGHTBITSHIFT_5,
	Shift6Right = ADC_RIGHTBITSHIFT_6,
	Shift7Right = ADC_RIGHTBITSHIFT_7,
	Shift8Right = ADC_RIGHTBITSHIFT_8,
	Shift9Right = ADC_RIGHTBITSHIFT_9,
	Shift10Right = ADC_RIGHTBITSHIFT_10,
	Shift11Right = ADC_RIGHTBITSHIFT_11,
};
enum AdcClockSourceDiv {
	PLL_Div1 = ADC_CLOCK_ASYNC_DIV1,
	PLL_Div2 = ADC_CLOCK_ASYNC_DIV2,
	PLL_Div4 = ADC_CLOCK_ASYNC_DIV4,
	PLL_Div6 = ADC_CLOCK_ASYNC_DIV6,
	PLL_Div8 = ADC_CLOCK_ASYNC_DIV8,
	PLL_Div10 = ADC_CLOCK_ASYNC_DIV10,
	PLL_Div12 = ADC_CLOCK_ASYNC_DIV12,
	PLL_Div16 = ADC_CLOCK_ASYNC_DIV16,
	PLL_Div32 = ADC_CLOCK_ASYNC_DIV32,
	PLL_Div64 = ADC_CLOCK_ASYNC_DIV64,
	PLL_Div128 = ADC_CLOCK_ASYNC_DIV128,
	PLL_Div256 = ADC_CLOCK_ASYNC_DIV256,
	APBClk_Div1 = ADC_CLOCK_SYNC_PCLK_DIV1,
	APBClk_Div2 = ADC_CLOCK_SYNC_PCLK_DIV2,
	APBClk_Div4 = ADC_CLOCK_SYNC_PCLK_DIV4,
};

struct AdcPeriphConf {
	static constexpr AdcPeriphNum adc_periph_num = AdcPeriphNum::_1;

	// Data type
	using ValueT = uint16_t;
	// enum DataSize { Byte, HalfWord, Word };
	// static constexpr DataSize data_size = HalfWord;

	static constexpr AdcResolution resolution = Bits16;

	// Ovesampling
	static constexpr bool oversample = false;
	static constexpr uint32_t oversampling_ratio = 0;
	static constexpr AdcOversampleRightBitShift oversampling_right_bitshift = NoShift;
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

	// Clock
	static constexpr AdcClockSourceDiv clock_div = PLL_Div2;
};

struct AdcChannelConf {
	PinNoInit pin;
	AdcChanNum adc_chan_num;
	uint32_t rank;
	AdcSamplingTime sampling_time = AdcSamplingTime::_32Cycles;
	// TODO: bool auto_set_rank = false;
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

		for (auto chan : ChanConfs)
			Pin init_adc_pin{chan.pin.gpio, chan.pin.pin, PinMode::Analog};

		Clocks::ADC::enable(get_ADC_base(ConfT::adc_periph_num));
		hadc = {
			.Instance = get_ADC_base(ConfT::adc_periph_num),
			.Init =
				{
					.ClockPrescaler = ConfT::clock_div,
					.Resolution = ConfT::resolution,
					.ScanConvMode = ADC_SCAN_ENABLE,
					.EOCSelection = ADC_EOC_SEQ_CONV,
					.LowPowerAutoWait = DISABLE,
					.ContinuousConvMode = ENABLE,
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
							.Ratio = MathTools::constrain(ConfT::oversampling_ratio, 1, 1024),
							.RightBitShift = ConfT::oversampling_right_bitshift,
							.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER,
							.OversamplingStopReset = ADC_REGOVERSAMPLING_RESUMED_MODE,
						},
				},
		};
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

		// Todo: allow auto rank.
		// Todo: what if we manually set rank but skip some numbers?
		for (auto chan : ChanConfs) {
			ADC_ChannelConfTypeDef adc_chan_conf = {
				.Channel = chan.adc_chan_num,
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
	DMA_HandleTypeDef hdma_adc1;

	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
