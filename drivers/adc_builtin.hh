#pragma once
#include "adc_builtin_conf.hh"
#include "adc_builtin_impl.hh"
#include "clocks.hh"
#include "periph.hh"
#include "stm32xx.h"
#include <array>

namespace mdrivlib
{

template<AdcPeriphConf ConfT>
class AdcDmaPeriph {
	using ValueT = typename ConfT::ValueT;

	AdcDmaImpl<ConfT> adc_impl;

public:
	// Initialize the ADC and DMA peripherals and create a regular sequence of channels
	// from the `ChanConfs` parameter.
	// The DMA will be run in circular (continuous) mode.
	// The given `dma_buffer` parameter must be friendly to being written upon by a DMA
	//(i.e. non-cacheable, in a proper section of memory)
	template<size_t N>
	AdcDmaPeriph(std::array<ValueT, N> &dma_buffer, std::array<AdcChannelConf, N> ChanConfs)
		: _dma_buffer{dma_buffer.data()}
		, num_channels{N} {
		static_assert(N <= 16, "This chip supports a maximum of 16 ADC channels in a sequence");

		Clocks::ADC::enable(static_cast<uint32_t>(ConfT::adc_periph_num));
		adc_impl.init_adc_dma(num_channels);
		adc_impl.init_adc_channels(ChanConfs);
	}

	// Start the DMA
	void start() {
		adc_impl.start(_dma_buffer, num_channels);
	}

	// Returns the last read value of the given ADC channel rank number
	// Note: This is not necessay, since you must provide the dma_buffer to the constructor,
	// therefore you can just read the dma_buffer whenever a value is needed.
	ValueT get_val(int chan) {
		return _dma_buffer[chan];
	}

private:
	ValueT *_dma_buffer;
	const size_t num_channels;
};

} // namespace mdrivlib
