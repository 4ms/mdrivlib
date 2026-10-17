#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"
#include <cstddef>

// IPCC register map for the STM32MP2. Shared by the Cortex-A35 and Cortex-M33.
// Port of target/stm32mp1/drivers/ipcc_regs.hh, verified on STM32MP257 hardware
// (stm32mp2-baremetal/ipcc).
//
// Differences from the MP1 version:
// - 16 channels per IPCC (MP15 has 6).
// - Two IPCC instances (IPCC1, IPCC2), so the register block base address is a
//   template parameter instead of the fixed IPCC_BASE.
// - Each CnCR interrupt enable is split in two: one bit arms the peripheral's
//   non-secure interrupt line (RXOIE/TXFIE), the other its secure line
//   (SECRXOIE/SECTXFIE). Which line a channel actually asserts is decided by
//   that channel's CnSECCFGR bit, so channel security, enable bit and IRQn must
//   all agree (see drivers/ipcc.hh).
//
// The bit-layout pattern is unchanged from MP1: every per-channel field is the
// channel-1 mask shifted left by (ChanNum - 1).
//
// Processor numbering is fixed by hardware. For IPCC1, Core<1> is the
// Cortex-A35 side of the mailbox and Core<2> is the Cortex-M33 side.
// "This core sets its own flag, and clears the other core's flag" is the whole
// model -- CnSCR is write-only with separate set and clear halves, so there are
// no read-modify-write races.

namespace mdrivlib
{
namespace IPCCRegs
{
template<uint32_t Base>
using Core1TXFreeISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_TXFIE>;
template<uint32_t Base>
using Core2TXFreeISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_TXFIE>;
template<uint32_t Base>
using Core1SecTXFreeISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_SECTXFIE>;
template<uint32_t Base>
using Core2SecTXFreeISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_SECTXFIE>;

// ISR for Core<2>'s SR Flag, per RM: "Associated with IPCC_C2TOC1SR"
template<uint32_t Base>
using Core1RXOccISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_RXOIE>;
template<uint32_t Base>
using Core1SecRXOccISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_SECRXOIE>;

// ISR for Core<1>'s SR Flag, per RM: "Associated with IPCC_C1TOC2SR"
template<uint32_t Base>
using Core2RXOccISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_RXOIE>;
template<uint32_t Base>
using Core2SecRXOccISREn = RegisterBits<ReadWrite, Base + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_SECRXOIE>;

template<uint32_t CoreNum, uint32_t Base>
struct Core {};

template<uint32_t Base>
struct Core<1, Base> {
	// Associated with this core's flag going low
	using TXFreeISREnable = Core1TXFreeISREn<Base>;
	using SecTXFreeISREnable = Core1SecTXFreeISREn<Base>;
	// Associated with other core's flag going high
	using RXOccISREnable = Core1RXOccISREn<Base>;
	using SecRXOccISREnable = Core1SecRXOccISREn<Base>;

	template<size_t ChanNum>
	struct Chan {
	private:
		static_assert(ChanNum >= 1 && ChanNum <= 16, "STM32MP2 IPCC has Channels 1 to 16");

		constexpr static auto Shift = (ChanNum - 1);
		constexpr static auto SetClearReg = Base + offsetof(IPCC_TypeDef, C1SCR);
		constexpr static auto StatusReg = Base + offsetof(IPCC_TypeDef, C1TOC2SR);
		constexpr static auto MaskReg = Base + offsetof(IPCC_TypeDef, C1MR);

		// Sets this core's SR.CHnF flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using SetFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C1SCR_CH1S << Shift>;

		// Clears other core's SR.CHnF flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using ClrFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C1SCR_CH1C << Shift>;

	public:
		using ChangeFlag = RegisterSetClear<SetFlag, ClrFlag>;
		using FlagStatus = RegisterBits<ReadOnly, StatusReg, IPCC_C1TOC2SR_CH1F << Shift>;

		// ISR for this core's SR Flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using FreeISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C1MR_CH1FM << Shift>;

		// ISR for other core's SR Flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using OccISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C1MR_CH1OM << Shift>;
	};
};

template<uint32_t Base>
struct Core<2, Base> {
	// Associated with this core's flag going low
	using TXFreeISREnable = Core2TXFreeISREn<Base>;
	using SecTXFreeISREnable = Core2SecTXFreeISREn<Base>;
	// Associated with other core's flag going high
	using RXOccISREnable = Core2RXOccISREn<Base>;
	using SecRXOccISREnable = Core2SecRXOccISREn<Base>;

	template<size_t ChanNum>
	struct Chan {
	private:
		static_assert(ChanNum >= 1 && ChanNum <= 16, "STM32MP2 IPCC has Channels 1 to 16");

		constexpr static auto Shift = (ChanNum - 1);
		constexpr static auto SetClearReg = Base + offsetof(IPCC_TypeDef, C2SCR);
		constexpr static auto StatusReg = Base + offsetof(IPCC_TypeDef, C2TOC1SR);
		constexpr static auto MaskReg = Base + offsetof(IPCC_TypeDef, C2MR);

		// Sets this core's SR.CHnF flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using SetFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C2SCR_CH1S << Shift>;
		// Clears other core's SR.CHnF flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using ClrFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C2SCR_CH1C << Shift>;

	public:
		using ChangeFlag = RegisterSetClear<SetFlag, ClrFlag>;
		using FlagStatus = RegisterBits<ReadOnly, StatusReg, IPCC_C2TOC1SR_CH1F << Shift>;

		// ISR for this Core's SR Flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using FreeISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C2MR_CH1FM << Shift>;

		// ISR for other Core's SR Flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using OccISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C2MR_CH1OM << Shift>;
	};
};

} // namespace IPCCRegs

} // namespace mdrivlib
