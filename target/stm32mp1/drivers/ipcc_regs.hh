#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
namespace IPCCRegs
{
using Core1TXFreeISREn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_TXFIE>;
using Core2TXFreeISREn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_TXFIE>;

//ISR for Core<2>'s SR Flag, per RM: "Associated with IPCC_C2TOC1SR"
using Core1RXOccISREn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C1CR), IPCC_C1CR_RXOIE>;

//ISR for Core<1>'s SR Flag, per RM: "Associated with IPCC_C1TOC2SR"
using Core2RXOccISREn = RegisterBits<ReadWrite, IPCC_BASE + offsetof(IPCC_TypeDef, C2CR), IPCC_C2CR_RXOIE>;

template<uint32_t CoreNum>
struct Core {};

template<>
struct Core<1> {
	// Associated with this core's flag going low
	using TXFreeISREnable = Core1TXFreeISREn;
	// Associated with other core's flag going high
	using RXOccISREnable = Core1RXOccISREn;

	template<size_t ChanNum>
	struct Chan {
	private:
		static_assert(ChanNum >= 1 && ChanNum <= 6, "STM32MP15x only has IPCC Channels 1 to 6");

		constexpr static auto Shift = (ChanNum - 1);
		constexpr static auto SetClearReg = IPCC_BASE + offsetof(IPCC_TypeDef, C1SCR);
		constexpr static auto StatusReg = IPCC_BASE + offsetof(IPCC_TypeDef, C1TOC2SR);
		constexpr static auto MaskReg = IPCC_BASE + offsetof(IPCC_TypeDef, C1MR);

		//Sets this core's SR.CHnF flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using SetFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C1SCR_CH1S << Shift>;

		//Clears other core's SR.CHnF flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using ClrFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C1SCR_CH1C << Shift>;

	public:
		using ChangeFlag = RegisterSetClear<SetFlag, ClrFlag>;
		using FlagStatus = RegisterBits<ReadOnly, StatusReg, IPCC_C1TOC2SR_CH1F << (ChanNum - 1)>;

		//ISR for this core's SR Flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using FreeISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C1MR_CH1FM << Shift>;

		//ISR for other core's SR Flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using OccISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C1MR_CH1OM << Shift>;
	};
};

template<>
struct Core<2> {
	// Associated with this core's flag going low
	using TXFreeISREnable = Core2TXFreeISREn;
	// Associated with other core's flag going high
	using RXOccISREnable = Core2RXOccISREn;

	template<size_t ChanNum>
	struct Chan {
	private:
		static_assert(ChanNum >= 1 && ChanNum <= 6, "STM32MP15x only has IPCC Channels 1 to 6");

		constexpr static auto Shift = (ChanNum - 1);
		constexpr static auto SetClearReg = IPCC_BASE + offsetof(IPCC_TypeDef, C2SCR);
		constexpr static auto StatusReg = IPCC_BASE + offsetof(IPCC_TypeDef, C2TOC1SR);
		constexpr static auto MaskReg = IPCC_BASE + offsetof(IPCC_TypeDef, C2MR);

		//Sets this core's SR.CHnF flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using SetFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C2SCR_CH1S << Shift>;
		//Clears other core's SR.CHnF flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using ClrFlag = RegisterBits<WriteOnly, SetClearReg, IPCC_C2SCR_CH1C << Shift>;

	public:
		using ChangeFlag = RegisterSetClear<SetFlag, ClrFlag>;
		using FlagStatus = RegisterBits<ReadOnly, StatusReg, IPCC_C2TOC1SR_CH1F << (ChanNum - 1)>;

		//ISR for this Core's SR Flag, per RM: "Associated with IPCC_C2TOC1SR_CHnF"
		using FreeISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C2MR_CH1FM << Shift>;

		//ISR for other Core's SR Flag, per RM: "Associated with IPCC_C1TOC2SR_CHnF"
		using OccISRMasked = RegisterBits<ReadWrite, MaskReg, IPCC_C2MR_CH1OM << Shift>;
	};
};

}; // namespace IPCCRegs

} // namespace mdrivlib
