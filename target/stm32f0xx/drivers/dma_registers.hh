#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
template<size_t ChannelNum>
struct DMA_ {
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
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_LIFCR_CTCIF0_Pos, 1>;
	using HalfTransferComplClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_LIFCR_CHTIF0_Pos, 1>;
	using TransferErrorClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_LIFCR_CTEIF0_Pos, 1>;
	using DirectModeErrorClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_LIFCR_CDMEIF0_Pos, 1>;
	using FIFOErrorClearFlag =
		RegisterSection<WriteOnly, GetIFCRBase(), GetBaseStreamShift() + DMA_LIFCR_CFEIF0_Pos, 1>;

	static constexpr uint32_t BASE = DMA1_BASE;
	static constexpr uint32_t STREAM_BASE = GetChannelBase();

	// Todo: Finish converting this class

	static constexpr uint32_t CR_Base = STREAM_BASE + offsetof(DMA_Stream_TypeDef, CR);
	using Enable = RegisterSection<ReadWrite, CR_Base, DMA_SxCR_EN_Pos, 1>;
	static constexpr uint32_t NDTR_Base = STREAM_BASE + offsetof(DMA_Stream_TypeDef, NDTR);
	// using...
	static constexpr uint32_t PAR_Base = STREAM_BASE + offsetof(DMA_Stream_TypeDef, PAR);
	// using...
	static constexpr uint32_t M0AR_Base = STREAM_BASE + offsetof(DMA_Stream_TypeDef, M0AR);
	// using...
	static constexpr uint32_t M1AR_Base = STREAM_BASE + offsetof(DMA_Stream_TypeDef, M1AR);
	// using...
	static constexpr uint32_t FCR_Base = STREAM_BASE + offsetof(DMA_Stream_TypeDef, FCR);
	// using...

	// Todo: add DMAMUX stuff

	// typedef struct
	// {
	//   __IO uint32_t  CCR;        /*!< DMA Multiplexer Channel x Control Register   */
	// }DMAMUX_Channel_TypeDef;

	// typedef struct
	// {
	//   __IO uint32_t  CSR;      /*!< DMA Channel Status Register     */
	//   __IO uint32_t  CFR;      /*!< DMA Channel Clear Flag Register */
	// }DMAMUX_ChannelStatus_TypeDef;

	// typedef struct
	// {
	//   __IO uint32_t  RGCR;        /*!< DMA Request Generator x Control Register   */
	// }DMAMUX_RequestGen_TypeDef;

	// typedef struct
	// {
	//   __IO uint32_t  RGSR;            /*!< DMAMUX Request Generator Status Register,           Address offset: 0x140
	//   */
	//   __IO uint32_t  RGCFR;           /*!< DMAMUX Request Generator Clear Flag Register,       Address offset: 0x144
	//   */ uint32_t       RESERVED0[169];  /*!< Reserved, 0x144 -> 0x144 */
	//   __IO uint32_t  HWCFGR2;         /*!< DMAMUX Configuration register 2,                    Address offset: 0x3EC
	//   */
	//   __IO uint32_t  HWCFGR1;         /*!< DMAMUX Configuration register 1,                    Address offset: 0x3F0
	//   */
	//   __IO uint32_t  VERR;            /*!< DMAMUX Verion Register,                             Address offset: 0x3F4
	//   */
	//   __IO uint32_t  IPDR;            /*!< DMAMUX Identification register,                     Address offset: 0x3F8
	//   */
	//   __IO uint32_t  SIDR;            /*!< DMAMUX Size Identification register,                Address offset: 0x3FC
	//   */

	// }DMAMUX_RequestGenStatus_TypeDef;
};
} // namespace mdrivlib
