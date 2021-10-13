#pragma once
#include "pin.hh"
#include "stm32mp1xx_hal_adc.h"
#include "stm32xx.h"

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
enum AdcSamplingTime : uint32_t {
	_1Cycle = ADC_SAMPLETIME_1CYCLE_5,
	_2Cycles = ADC_SAMPLETIME_2CYCLES_5,
	_8Cycles = ADC_SAMPLETIME_8CYCLES_5,
	_16Cycles = ADC_SAMPLETIME_16CYCLES_5,
	_32Cycles = ADC_SAMPLETIME_32CYCLES_5,
	_64Cycles = ADC_SAMPLETIME_64CYCLES_5,
	_387Cycles = ADC_SAMPLETIME_387CYCLES_5,
	_810Cycles = ADC_SAMPLETIME_810CYCLES_5,
};
enum AdcResolution : uint32_t {
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
enum AdcClockSourceDiv : uint32_t {
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
} // namespace mdrivlib
