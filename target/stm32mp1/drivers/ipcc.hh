#pragma once
#include "drivers/register_access.hh"
#include "stm32mp1xx_ll_ipcc.h"

namespace mdrivlib
{
struct IPCCRegs {
	using Core1TXFreeIntEn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_TXFIE>;
	using Core2TXFreeIntEn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_TXFIE>;
	using Core1RXOccIntEn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_RXOIE>;
	using Core2RXOccIntEn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_RXOIE>;

	template<size_t CoreNum>
	struct Core;

	template<>
	struct Core<1> {
		using TXFreeIntEn = Core1TXFreeIntEn;
		using RXOccIntEn = Core1RXOccIntEn;

		// Auto-generated with:
		// s/\v#define IPCC_([^_]{-})_([^_]{-}) /using \2 = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, \1), IPCC_\1_\2>;\r\0/
		// RW vs RO access modified manually
		using CH1C = RegisterBits<WriteOnly, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH1C>;
		using CH1F = RegisterBits<ReadOnly, IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR), IPCC_C1TOC2SR_CH1F>;
		using CH1FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH1FM>;
		using CH1OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH1OM>;
		using CH1S = RegisterBits<WriteOnly, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH1S>;

		using CH2C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH2C>;
		using CH2F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR), IPCC_C1TOC2SR_CH2F>;
		using CH2FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH2FM>;
		using CH2OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH2OM>;
		using CH2S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH2S>;
		using CH3C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH3C>;
		using CH3F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR), IPCC_C1TOC2SR_CH3F>;
		using CH3FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH3FM>;
		using CH3OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH3OM>;
		using CH3S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH3S>;
		using CH4C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH4C>;
		using CH4F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR), IPCC_C1TOC2SR_CH4F>;
		using CH4FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH4FM>;
		using CH4OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH4OM>;
		using CH4S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH4S>;
		using CH5C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH5C>;
		using CH5F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR), IPCC_C1TOC2SR_CH5F>;
		using CH5FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH5FM>;
		using CH5OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH5OM>;
		using CH5S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH5S>;
		using CH6C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH6C>;
		using CH6F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR), IPCC_C1TOC2SR_CH6F>;
		using CH6FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH6FM>;
		using CH6OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1MR), IPCC_C1MR_CH6OM>;
		using CH6S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH6S>;

		template<size_t ChanNum>
		struct Chan {
			static_assert(ChanNum >= 1 && ChanNum <= 6, "Channel 1 to 6 only allowed");
			using SetFlag =
				RegisterBits<WriteOnly, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH1S << (ChanNum - 1)>;
			using ClrFlag =
				RegisterBits<WriteOnly, IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR), IPCC_C1SCR_CH1C << (ChanNum - 1)>;
			using ChangeFlag = RegisterSetClear<SetFlag, ClrFlag>;
		};
	};

	template<>
	struct Core<2> {
		using TXFreeIntEn = Core2TXFreeIntEn;
		using RXOccIntEn = Core2RXOccIntEn;

		// Auto-generated with:
		// s/\v#define IPCC_([^_]{-})_([^_]{-}) /using \2 = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, \1), IPCC_\1_\2>;\r\0/
		using CH1C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH1C>;
		using CH1F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR), IPCC_C2TOC1SR_CH1F>;
		using CH1FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH1FM>;
		using CH1OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH1OM>;
		using CH1S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH1S>;
		using CH2C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH2C>;
		using CH2F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR), IPCC_C2TOC1SR_CH2F>;
		using CH2FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH2FM>;
		using CH2OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH2OM>;
		using CH2S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH2S>;
		using CH3C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH3C>;
		using CH3F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR), IPCC_C2TOC1SR_CH3F>;
		using CH3FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH3FM>;
		using CH3OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH3OM>;
		using CH3S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH3S>;
		using CH4C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH4C>;
		using CH4F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR), IPCC_C2TOC1SR_CH4F>;
		using CH4FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH4FM>;
		using CH4OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH4OM>;
		using CH4S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH4S>;
		using CH5C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH5C>;
		using CH5F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR), IPCC_C2TOC1SR_CH5F>;
		using CH5FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH5FM>;
		using CH5OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH5OM>;
		using CH5S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH5S>;
		using CH6C = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH6C>;
		using CH6F = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR), IPCC_C2TOC1SR_CH6F>;
		using CH6FM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH6FM>;
		using CH6OM = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2MR), IPCC_C2MR_CH6OM>;
		using CH6S = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR), IPCC_C2SCR_CH6S>;
	};
	//Usage: Core<N>::TXFreeIntEn::set();
};

template<size_t Core>
struct IPCC_;

template<>
struct IPCC_<1> {
	static void EnableTXFreeIntr() {
		IPCC->C1CR = IPCC->C1CR | IPCC_C1CR_TXFIE;
	}
	static void DisbleTXFreeIntr() {
		IPCC->C1CR = IPCC->C1CR | IPCC_C1CR_TXFIE;
	}
};

template<>
struct IPCC_<2> {};

} // namespace mdrivlib
