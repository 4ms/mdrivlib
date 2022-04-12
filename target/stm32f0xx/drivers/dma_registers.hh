#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
template<size_t PeriphNum, size_t ChannelNum>
struct DMA_ {
	static_assert(PeriphNum == 1, "STM32F030x6 only has DMA1");
	static_assert(ChannelNum >= 1 && ChannelNum <= 5, "Invalid DMA channel number, F030x6 has only channels 1-5");

	static constexpr uint32_t GetChannelBase() {
		return ChannelNum == 1 ? DMA1_Channel1_BASE
			 : ChannelNum == 2 ? DMA1_Channel2_BASE
			 : ChannelNum == 3 ? DMA1_Channel3_BASE
			 : ChannelNum == 4 ? DMA1_Channel4_BASE
			 : ChannelNum == 5 ? DMA1_Channel5_BASE
							   : 0;
	}

	static constexpr uint32_t GetIFCRBase() {
		return BASE + offsetof(DMA_TypeDef, IFCR);
	}

	static constexpr uint32_t GetISRBase() {
		return BASE + offsetof(DMA_TypeDef, ISR);
	}

	// returns the bit number of the lowest register for this stream
	// in the base DMA's LIFCR/HIFCR or LISR/HISR
	static constexpr uint32_t GetBaseStreamShift() {
		return (ChannelNum - 1) * 4;
	}

	using TransferComplClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_IFCR_CTCIF1_Pos, 1>;
	using HalfTransferComplClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_IFCR_CHTIF1_Pos, 1>;
	using TransferErrorClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_IFCR_CTEIF1_Pos, 1>;

	static constexpr uint32_t BASE = DMA1_BASE;
	static constexpr uint32_t STREAM_BASE = GetChannelBase();

	// Todo: Finish converting these registers

	static constexpr uint32_t CR_Base = STREAM_BASE + offsetof(DMA_Channel_TypeDef, CCR);
	using Enable = RegisterSection<ReadWrite, CR_Base, DMA_CCR_EN_Pos, 1>;

	static constexpr uint32_t NDTR_Base = STREAM_BASE + offsetof(DMA_Channel_TypeDef, CNDTR);
	// using...
	static constexpr uint32_t PAR_Base = STREAM_BASE + offsetof(DMA_Channel_TypeDef, CPAR);
	// using...
	static constexpr uint32_t M0AR_Base = STREAM_BASE + offsetof(DMA_Channel_TypeDef, CMAR);
	// using...
};
} // namespace mdrivlib
