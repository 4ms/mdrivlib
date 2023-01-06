#pragma once
#include "dma_config_struct.hh"
#include "pin.hh"
#include "stm32xx.h"
#include <concepts>

//TODO: Less target-specific #defines by not setting the enum members equal to a target-specific value
//In the target adc driver, interpret the enum as the target-specific value. Invalid values can raise a compile-time error
//Not so easy to do with AdcClockSourceDiv
namespace mdrivlib
{

enum class AdcPeriphNum { _1, _2, _3 };

enum class AdcChanNum : uint32_t {
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
#if defined(ADC_CHANNEL_18)
	_18 = ADC_CHANNEL_18,
#endif
#if defined(ADC_CHANNEL_19)
	_19 = ADC_CHANNEL_19,
#endif
};

enum AdcSamplingTime : uint32_t {
#if defined(STM32MP1)
	_1Cycle = ADC_SAMPLETIME_1CYCLE_5,
	_2Cycles = ADC_SAMPLETIME_2CYCLES_5,
	_8Cycles = ADC_SAMPLETIME_8CYCLES_5,
	_16Cycles = ADC_SAMPLETIME_16CYCLES_5,
	_32Cycles = ADC_SAMPLETIME_32CYCLES_5,
	_64Cycles = ADC_SAMPLETIME_64CYCLES_5,
	_387Cycles = ADC_SAMPLETIME_387CYCLES_5,
	_810Cycles = ADC_SAMPLETIME_810CYCLES_5,
#elif defined(STM32F030)
	_1Cycle = ADC_SAMPLETIME_1CYCLE_5,
	_7Cycles = ADC_SAMPLETIME_7CYCLES_5,
	_13Cycles = ADC_SAMPLETIME_13CYCLES_5,
	_28Cycles = ADC_SAMPLETIME_28CYCLES_5,
	_41Cycles = ADC_SAMPLETIME_41CYCLES_5,
	_55Cycles = ADC_SAMPLETIME_55CYCLES_5,
	_71Cycles = ADC_SAMPLETIME_71CYCLES_5,
	_239Cycles = ADC_SAMPLETIME_239CYCLES_5,
#elif defined(STM32F7)
	_3Cycles = ADC_SAMPLETIME_3CYCLES,
	_15Cycles = ADC_SAMPLETIME_15CYCLES,
	_28Cycles = ADC_SAMPLETIME_28CYCLES,
	_56Cycles = ADC_SAMPLETIME_56CYCLES,
	_84Cycles = ADC_SAMPLETIME_84CYCLES,
	_112Cycles = ADC_SAMPLETIME_112CYCLES,
	_144Cycles = ADC_SAMPLETIME_144CYCLES,
	_480Cycles = ADC_SAMPLETIME_480CYCLES,
#endif
};

enum AdcResolution : uint32_t {
	Bits8 = ADC_RESOLUTION_8B,
	Bits10 = ADC_RESOLUTION_10B,
	Bits12 = ADC_RESOLUTION_12B,
#if defined(ADC_RESOLUTION_16B)
	Bits14 = ADC_RESOLUTION_14B,
	Bits16 = ADC_RESOLUTION_16B
#endif
};

enum AdcOversampleRightBitShift : uint32_t {
#if defined(ADC_RIGHTBITSHIFT_NONE)
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
#else
	NoShift = 0,
#endif
};

enum AdcDataAlign : uint32_t {
	AlignLeft = ADC_DATAALIGN_LEFT,
	AlignRight = ADC_DATAALIGN_RIGHT,
};

enum AdcClockSourceDiv : uint32_t {
#if defined(ADC_CLOCK_ASYNC_DIV2)
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
#endif
	APBClk_Div2 = ADC_CLOCK_SYNC_PCLK_DIV2,
	APBClk_Div4 = ADC_CLOCK_SYNC_PCLK_DIV4,
#if defined(ADC_CLOCK_SYNC_PCLK_DIV6)
	APBClk_Div6 = ADC_CLOCK_SYNC_PCLK_DIV6,
#endif
#if defined(ADC_CLOCK_SYNC_PCLK_DIV8)
	APBClk_Div8 = ADC_CLOCK_SYNC_PCLK_DIV8,
#endif
};

struct DefaultAdcPeriphConf {
	static constexpr AdcPeriphNum adc_periph_num = AdcPeriphNum::_1;

	// Data type
	using ValueT = uint16_t;
	// enum DataSize { Byte, HalfWord, Word };
	// static constexpr DataSize data_size = HalfWord;

	static constexpr AdcResolution resolution = Bits12;

	static constexpr AdcDataAlign align = AlignRight;

	// Ovesampling
	static constexpr bool oversample = false;
	static constexpr uint32_t oversampling_ratio = 0;
	static constexpr AdcOversampleRightBitShift oversampling_right_bitshift = NoShift;
	// TODO: other os config opts?

	// Clock
	static constexpr AdcClockSourceDiv clock_div = APBClk_Div2;

	// DMA Conf
	static constexpr bool use_dma = true;
	static constexpr bool use_dma_irq = false;
	struct DmaConf : DefaultDMAConf {
		static constexpr auto dir = Periph2Mem;
		static constexpr auto circular = true;
		static constexpr auto transfer_size_mem = HalfWord;
		static constexpr auto transfer_size_periph = HalfWord;
		static constexpr auto dma_priority = Low;
		static constexpr auto mem_inc = true;
		static constexpr auto periph_inc = false;
		static constexpr auto enable_fifo = false;
	};

	static constexpr bool enable_end_of_sequence_isr = false;
	static constexpr bool enable_end_of_conversion_isr = false;
};

template<typename T>
concept AdcPeriphConf = requires(T) {
	requires std::derived_from<T, DefaultAdcPeriphConf>;
	requires std::derived_from<typename T::DmaConf, DefaultDMAConf>;
};

struct AdcChannelConf {
	PinNoInit pin;
	AdcChanNum adc_chan_num;
	uint32_t rank;
	AdcSamplingTime sampling_time; // = AdcSamplingTime::_1Cycle;
								   // TODO: bool auto_set_rank = false;
								   // TODO: Single/diff
								   // TODO: offset
};

} // namespace mdrivlib
