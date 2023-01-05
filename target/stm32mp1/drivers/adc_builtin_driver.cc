

// DEPRECATED!!

/*
 * adc_builtin_driver.c - adc driver for built-in adcs
 * Uses DMA to dump ADC values into buffers
 *
 * Author: Dan Green (danngreen1@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * See http://creativecommons.org/licenses/MIT/ for more information.
 *
 * -----------------------------------------------------------------------------
 */
#include "adc_builtin_driver.hh"
#include "interrupt_control.hh"
#include "stm32xx.h"

namespace mdrivlib
{
template class AdcPeriph<AdcPeriphNum::_1>;
template class AdcPeriph<AdcPeriphNum::_2>;
template class AdcPeriph<AdcPeriphNum::_3>;

template<AdcPeriphNum p>
AdcPeriph<p> &AdcPeriph<p>::AdcInstance() {
	static AdcPeriph<p> Adc_;
	return Adc_;
}

// Todo: Use ConfT:
// -IRQn
// -NumChannels
// -uint8_t channel_nums[NumChannels];
// -SampleTime []
// -Periphs []
// or: struct AdcChan {AdcPeriph periph, uint8_t AdcChanNum, SampleTime sampletime};
//     AdcChan channels[NumChannels];
// resolution (default 12? 16?)
// Todo: Fix rampant use of static?
//

template<AdcPeriphNum p>
AdcPeriph<p>::AdcPeriph() {
	Clocks::ADC::enable(ADCx);

	dma_buffer_ = default_dma_buffer_;

	num_channels_ = 0;
	LL_ADC_Disable(ADCx);

	InterruptControl::disable_irq(ADC1_IRQn); // ConfT::IRQn

	// Todo: ConfT::CommonClockSync
	if constexpr (ADC123_COMMON_BASE != 0)
		LL_ADC_SetCommonClock(ADC123_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
	else if constexpr (ADC12_COMMON_BASE != 0 && (p == AdcPeriphNum::_1 || p == AdcPeriphNum::_2))
		LL_ADC_SetCommonClock(ADC12_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV4);
	else if constexpr (ADC1_COMMON_BASE != 0 && p == AdcPeriphNum::_1)
		LL_ADC_SetCommonClock(ADC1_COMMON, LL_ADC_CLOCK_ASYNC_DIV64);
	else if constexpr (ADC2_COMMON_BASE != 0 && p == AdcPeriphNum::_2)
		LL_ADC_SetCommonClock(ADC2_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
	else if constexpr (ADC3_COMMON_BASE != 0 && p == AdcPeriphNum::_3)
		LL_ADC_SetCommonClock(ADC3_COMMON, LL_ADC_CLOCK_SYNC_PCLK_DIV2);

	LL_ADC_DisableDeepPowerDown(ADCx);
	LL_ADC_SetResolution(ADCx, LL_ADC_RESOLUTION_16B);

	// #if !defined(STM32H755xx) && !defined(STM32H745xx) && !defined(STM32MP1)
	// 	LL_ADC_SetDataAlignment(get_ADC_base(p), LL_ADC_DATA_ALIGN_RIGHT);
	// 	LL_ADC_SetSequencersScanMode(get_ADC_base(p), LL_ADC_SEQ_SCAN_ENABLE);
	// #endif

	// #if defined(STM32F7)
	// 	LL_ADC_REG_InitTypeDef adcreginit;

	// 	adcreginit.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	// 	adcreginit.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	// 	adcreginit.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	// 	adcreginit.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	// 	adcreginit.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	// 	LL_ADC_REG_Init(get_ADC_base(p), &adcreginit);
	// 	LL_ADC_REG_SetFlagEndOfConversion(get_ADC_base(p), LL_ADC_REG_FLAG_EOC_SEQUENCE_CONV);
	// #endif
}

constexpr uint32_t adc_channum_to_ll_channel(const uint32_t x) {
	return x == 0  ? LL_ADC_CHANNEL_0
		 : x == 1  ? LL_ADC_CHANNEL_1
		 : x == 2  ? LL_ADC_CHANNEL_2
		 : x == 3  ? LL_ADC_CHANNEL_3
		 : x == 4  ? LL_ADC_CHANNEL_4
		 : x == 5  ? LL_ADC_CHANNEL_5
		 : x == 6  ? LL_ADC_CHANNEL_6
		 : x == 7  ? LL_ADC_CHANNEL_7
		 : x == 8  ? LL_ADC_CHANNEL_8
		 : x == 9  ? LL_ADC_CHANNEL_9
		 : x == 10 ? LL_ADC_CHANNEL_10
		 : x == 11 ? LL_ADC_CHANNEL_11
		 : x == 12 ? LL_ADC_CHANNEL_12
		 : x == 13 ? LL_ADC_CHANNEL_13
		 : x == 14 ? LL_ADC_CHANNEL_14
		 : x == 15 ? LL_ADC_CHANNEL_15
		 : x == 16 ? LL_ADC_CHANNEL_16
		 : x == 17 ? LL_ADC_CHANNEL_17
		 : x == 18 ? LL_ADC_CHANNEL_18
		 : x == 19 ? LL_ADC_CHANNEL_19
				   : LL_ADC_CHANNEL_0;
}

constexpr uint32_t adc_number_to_ll_rank(const uint32_t x) {
	return x == 0  ? LL_ADC_REG_RANK_1
		 : x == 1  ? LL_ADC_REG_RANK_2
		 : x == 2  ? LL_ADC_REG_RANK_3
		 : x == 3  ? LL_ADC_REG_RANK_4
		 : x == 4  ? LL_ADC_REG_RANK_5
		 : x == 5  ? LL_ADC_REG_RANK_6
		 : x == 6  ? LL_ADC_REG_RANK_7
		 : x == 7  ? LL_ADC_REG_RANK_8
		 : x == 8  ? LL_ADC_REG_RANK_9
		 : x == 9  ? LL_ADC_REG_RANK_10
		 : x == 10 ? LL_ADC_REG_RANK_11
		 : x == 11 ? LL_ADC_REG_RANK_12
		 : x == 12 ? LL_ADC_REG_RANK_13
		 : x == 13 ? LL_ADC_REG_RANK_14
		 : x == 14 ? LL_ADC_REG_RANK_15
		 : x == 15 ? LL_ADC_REG_RANK_16
				   : 0;
}
constexpr uint32_t _LL_ADC_DECIMAL_NB_TO_REG_SEQ_LENGTH(const uint8_t x) {
	return (x << ADC_SQR1_L_Pos);
}

template<AdcPeriphNum p>
void AdcPeriph<p>::add_channel(const AdcChanNum channel, const uint32_t sampletime) {
	auto channel_int = static_cast<uint32_t>(channel);
	LL_ADC_REG_SetSequencerRanks(ADCx, adc_number_to_ll_rank(num_channels_), adc_channum_to_ll_channel(channel_int));
	LL_ADC_REG_SetSequencerLength(ADCx, _LL_ADC_DECIMAL_NB_TO_REG_SEQ_LENGTH(num_channels_));
	LL_ADC_SetChannelSamplingTime(ADCx, adc_channum_to_ll_channel(channel_int), sampletime);
	LL_ADC_REG_SetDataTransferMode(ADCx, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
	LL_ADC_REG_SetContinuousMode(ADCx, LL_ADC_REG_CONV_CONTINUOUS);

	ranks_[channel_int] = num_channels_++;
}

template<AdcPeriphNum p>
void AdcPeriph<p>::init_dma(const DMA_LL_Config &dma_defs, uint16_t *dma_buffer) {
	dma_buffer_ = dma_buffer;

	if (!num_channels_)
		return;

	Clocks::DMA::enable(dma_defs.DMAx);

	LL_DMA_ConfigTransfer(dma_defs.DMAx,
						  dma_defs.stream,
						  LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
							  LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_HALFWORD | LL_DMA_MDATAALIGN_HALFWORD |
							  LL_DMA_PRIORITY_HIGH);
	LL_DMA_ConfigFifo(dma_defs.DMAx, dma_defs.stream, LL_DMA_FIFOMODE_ENABLE, LL_DMA_FIFOTHRESHOLD_1_2);
	LL_DMA_SetMemoryBurstxfer(dma_defs.DMAx, dma_defs.stream, LL_DMA_MBURST_SINGLE);
	LL_DMA_SetPeriphBurstxfer(dma_defs.DMAx, dma_defs.stream, LL_DMA_PBURST_SINGLE);
	LL_DMA_ConfigAddresses(dma_defs.DMAx,
						   dma_defs.stream,
						   LL_ADC_DMA_GetRegAddr(ADCx, LL_ADC_DMA_REG_REGULAR_DATA),
						   (uint32_t)(dma_buffer_),
						   LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetDataLength(dma_defs.DMAx, dma_defs.stream, num_channels_);
	// LL_DMA_EnableIT_TC(dma_defs.DMAx, dma_defs.stream);
	// LL_DMA_DisableIT_HT(dma_defs.DMAx, dma_defs.stream);
	// LL_DMA_EnableIT_TE(dma_defs.DMAx, dma_defs.stream);
	LL_DMA_SetPeriphRequest(dma_defs.DMAx, dma_defs.stream, LL_DMAMUX1_REQ_ADC1);

	LL_DMA_EnableStream(dma_defs.DMAx, dma_defs.stream);

	DMA_IRQn = dma_defs.IRQn;
	DMA_IRQ_pri = dma_defs.pri;
	DMA_IRQ_subpri = dma_defs.subpri;
}

template<AdcPeriphNum p>
void AdcPeriph<p>::enable_DMA_IT() {
	InterruptControl::set_irq_priority(DMA_IRQn, DMA_IRQ_pri, DMA_IRQ_subpri);
	InterruptControl::enable_irq(DMA_IRQn);
}

template<AdcPeriphNum p>
void AdcPeriph<p>::start_adc() {
	LL_ADC_Enable(ADCx);
	LL_ADC_REG_StartConversion(ADCx);
}
} // namespace mdrivlib
