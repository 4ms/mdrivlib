#pragma once

#include "gic_base.hh"
#include <cstdint>

using IRQn_Type = uint32_t;

// TODO: use gic_basic.c from ARM instead of this

// GICD
struct GICD_t {
	uint32_t CTLR;			  // Offset: 0x000 (R/W) Distributor Control Register
	const uint32_t TYPER;	  // Offset: 0x004 (R/ ) Interrupt Controller Type Register
	const uint32_t IIDR;	  // Offset: 0x008 (R/ ) Distributor Implementer Identification Register
	uint32_t _pad0;			  // TYPER2
	uint32_t STATUSR;		  // Offset: 0x010 (R/W) Error Reporting Status Register, optional
	uint32_t _pad1[11];		  //
	uint32_t SETSPI_NSR;	  // Offset: 0x040 ( /W) Set SPI Register
	uint32_t _pad2;			  //
	uint32_t CLRSPI_NSR;	  // Offset: 0x048 ( /W) Clear SPI Register
	uint32_t _pad3;			  //
	uint32_t SETSPI_SR;		  // Offset: 0x050 ( /W) Set SPI, Secure Register
	uint32_t _pad4;			  //
	uint32_t CLRSPI_SR;		  // Offset: 0x058 ( /W) Clear SPI, Secure Register
	uint32_t _pad5[9];		  //
	uint32_t IGROUPR[32];	  // Offset: 0x080 (R/W) Interrupt Group Registers
	uint32_t ISENABLER[32];	  // Offset: 0x100 (R/W) Interrupt Set-Enable Registers
	uint32_t ICENABLER[32];	  // Offset: 0x180 (R/W) Interrupt Clear-Enable Registers
	uint32_t ISPENDR[32];	  // Offset: 0x200 (R/W) Interrupt Set-Pending Registers
	uint32_t ICPENDR[32];	  // Offset: 0x280 (R/W) Interrupt Clear-Pending Registers
	uint32_t ISACTIVER[32];	  // Offset: 0x300 (R/W) Interrupt Set-Active Registers
	uint32_t ICACTIVER[32];	  // Offset: 0x380 (R/W) Interrupt Clear-Active Registers
	uint32_t IPRIORITYR[255]; // Offset: 0x400 (R/W) Interrupt Priority Registers
	uint32_t _pad6;			  //
	uint32_t ITARGETSR[255];  // Offset: 0x800 (R/W) Interrupt Targets Registers
	uint32_t _pad7;			  //
	uint32_t ICFGR[64];		  // Offset: 0xC00 (R/W) Interrupt Configuration Registers
	uint32_t IGRPMODR[32];	  // Offset: 0xD00 (R/W) Interrupt Group Modifier Registers
	uint32_t _pad8[32];		  //
	uint32_t NSACR[64];		  // Offset: 0xE00 (R/W) Non-secure Access Control Registers
	uint32_t SGIR;			  // Offset: 0xF00 ( /W) Software Generated Interrupt Register
	uint32_t _pad9[3];		  //
	uint32_t CPENDSGIR[4];	  // Offset: 0xF10 (R/W) SGI Clear-Pending Registers
	uint32_t SPENDSGIR[4];	  // Offset: 0xF20 (R/W) SGI Set-Pending Registers
	uint32_t _pad10[5236];	  //
	uint64_t IROUTER[988];	  // Offset: 0x6100(R/W) Interrupt Routing Registers
};

struct GICR_t {
	uint32_t CTLR;
	uint32_t IIDR;
	uint32_t TYPER;
	uint32_t STATUSR;
	uint32_t WAKER;

	uint32_t skip1[26];

	uint32_t IGROUPR[32];
	uint32_t ISENABLER[32];
	uint32_t ICENABLER[32];
	uint32_t ISPENDR[32];
	uint32_t ICPENDR[32];
	uint32_t ISACTIVER[32]; // 0x300
	uint32_t ICACTIVER[32];

	// TODO...

	void Wake() volatile {
		enum : uint32_t { ProcSleep = (1 << 1), ChildrenAsleep = (1 << 2) };

		WAKER &= ~ProcSleep;
		while (WAKER & (1 << ChildrenAsleep))
			;
	}
};

namespace HW
{

static inline volatile GICR_t *const GICRedist = reinterpret_cast<GICR_t *>(GIC_REDISTRIBUTOR_BASE);
static inline volatile GICD_t *const GICDistributor = reinterpret_cast<GICD_t *>(GIC_DISTRIBUTOR_BASE);

} // namespace HW

static inline void GIC_EnableICCAccess(void) {
	uint64_t reg = 1;
	asm volatile("msr ICC_SRE_EL2, %0\n\t" : : "r"(reg) : "memory");
}
//
// Groups
//

static inline void GIC_EnableGroup0(void) {
	HW::GICDistributor->CTLR |= 1U;
	uint64_t reg = 0b1;
	asm volatile("msr ICC_IGRPEN0_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_EnableGroup1NS(void) {
	HW::GICDistributor->CTLR |= (1 << 1);
	uint64_t reg = 0b1;
	asm volatile("msr ICC_IGRPEN1_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_EnableGroup1S(void) {
	HW::GICDistributor->CTLR |= (1 << 2);
	uint64_t reg = 0b1;
	asm volatile("msr ICC_IGRPEN1_EL1, %0\n\t" : : "r"(reg) : "memory");
}

// static inline void GIC_DisableGroup0(void) {
// 	HW::GICDistributor->CTLR &= ~1U;
// }
// static inline void GIC_DisableGroup1NS(void) {
// 	HW::GICDistributor->CTLR &= ~(1 << 1);
// }
// static inline void GIC_DisableGroup1S(void) {
// 	HW::GICDistributor->CTLR &= ~(1 << 2);
// }

//
// IDs
//

static inline uint32_t GIC_DistributorInfo(void) {
	return (HW::GICDistributor->TYPER);
}

static inline uint32_t GIC_DistributorImplementer(void) {
	return (HW::GICDistributor->IIDR);
}

//
// Target
//

static inline void GIC_SetTarget(IRQn_Type IRQn, uint32_t cpu_target) {
	uint32_t mask = HW::GICDistributor->ITARGETSR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
	HW::GICDistributor->ITARGETSR[IRQn / 4U] = mask | ((cpu_target & 0xFFUL) << ((IRQn % 4U) * 8U));
}

static inline uint32_t GIC_GetTarget(IRQn_Type IRQn) {
	return (HW::GICDistributor->ITARGETSR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

//
// Enable/Disable
//

static inline void GIC_EnableIRQ(IRQn_Type IRQn) {
	HW::GICDistributor->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
	HW::GICRedist->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

static inline uint32_t GIC_GetEnableIRQ(IRQn_Type IRQn) {
	return (HW::GICDistributor->ISENABLER[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}

static inline void GIC_DisableIRQ(IRQn_Type IRQn) {
	HW::GICDistributor->ICENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

//
// Pending
//

static inline uint32_t GIC_GetPendingIRQ(IRQn_Type IRQn) {
	uint32_t pend;

	if (IRQn >= 16U) {
		pend = (HW::GICDistributor->ISPENDR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
	} else {
		// INTID 0-15 Software Generated Interrupt
		pend = (HW::GICDistributor->SPENDSGIR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
		// No CPU identification offered
		if (pend != 0U) {
			pend = 1U;
		} else {
			pend = 0U;
		}
	}

	return (pend);
}

static inline void GIC_SetPendingIRQ(IRQn_Type IRQn) {
	if (IRQn >= 16U) {
		HW::GICDistributor->ISPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
	} else {
		// INTID 0-15 Software Generated Interrupt
		HW::GICDistributor->SPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
	}
}

static inline void GIC_ClearPendingIRQ(IRQn_Type IRQn) {
	if (IRQn >= 16U) {
		HW::GICDistributor->ICPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
	} else {
		// INTID 0-15 Software Generated Interrupt
		HW::GICDistributor->CPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
	}
}

//
// Edge/Level
//

// int_config: Bit 1: 0 - level sensitive, 1 - edge triggered
enum InterruptConfig : uint32_t { Level = 0, Edge = 2 };
static inline void GIC_SetConfiguration(IRQn_Type IRQn, InterruptConfig int_config) {
	uint32_t icfgr = HW::GICDistributor->ICFGR[IRQn / 16U];
	uint32_t shift = (IRQn % 16U) << 1U;

	icfgr &= (~(3U << shift));
	icfgr |= (int_config << shift);

	HW::GICDistributor->ICFGR[IRQn / 16U] = icfgr;
}

/** \brief Get the interrupt configuration from the GIC's ICFGR register.
 * \param [in] IRQn Interrupt to acquire the configuration for.
 * \return Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
 *                                 Bit 1: 0 - level sensitive, 1 - edge triggered
 */
static inline uint32_t GIC_GetConfiguration(IRQn_Type IRQn) {
	return (HW::GICDistributor->ICFGR[IRQn / 16U] >> ((IRQn % 16U) >> 1U));
}

//
// Priority
//
static inline void GIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
	uint32_t mask = HW::GICDistributor->IPRIORITYR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
	HW::GICDistributor->IPRIORITYR[IRQn / 4U] = mask | ((priority & 0xFFUL) << ((IRQn % 4U) * 8U));
}

static inline uint32_t GIC_GetPriority(IRQn_Type IRQn) {
	return (HW::GICDistributor->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

static inline void GIC_SetRoutingMode(IRQn_Type IRQn) {
	HW::GICDistributor->IROUTER[IRQn / 32U] = 1U << (IRQn % 32U);
}

static inline uint32_t GIC_GetRoutingMode(IRQn_Type IRQn) {
	return (HW::GICDistributor->IROUTER[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}

//
// Acknowledge
//

static inline IRQn_Type GIC_AcknowledgePendingGroup0(void) {
	uint64_t reg;
	__asm__ __volatile__("mrs %0, ICC_IAR0_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline uint64_t GIC_AcknowledgePendingGroup1() {
	uint64_t reg;
	__asm__ __volatile__("mrs %0, ICC_IAR1_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

// EOIR always provides priority drop.
// Two-step mode means ICC_DIR_EL1 provides interrupt deactivation,
// otherwise EOIR also does interrupt deactivation
static inline void GIC_SetEOIModeTwoStep(bool two_step) {
	uint64_t reg;
	asm volatile("mrs %0, ICC_CTLR_EL1\n\t" : "=r"(reg) : : "memory");
	if (two_step)
		reg |= (1 << 1);
	else
		reg &= ~(1 << 1);
	asm volatile("msr ICC_CTLR_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_EndInterruptGroup0(IRQn_Type IRQn) {
	uint64_t reg = IRQn;
	asm volatile("msr ICC_EOIR0_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_EndInterruptGroup1(uint64_t IRQn) {
	asm volatile("msr ICC_EOIR1_EL1, %0\n\t" : : "r"(IRQn) : "memory");
}

static inline void GIC_DeactivateInterrupt(uint64_t IRQn) {
	asm volatile("msr ICC_DIR_EL1, %0\n\t" : : "r"(IRQn) : "memory");
}

//
// Priority Mask and Binary Point
//

static inline void GIC_SetInterfacePriorityMask(uint64_t priority) {
	asm volatile("msr ICC_PMR_EL1, %0\n\t" : : "r"(priority) : "memory");
}

static inline uint32_t GIC_GetInterfacePriorityMask(void) {
	uint64_t reg;
	asm volatile("mrs %0, ICC_PMR_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline void GIC_SetBinaryPoint(uint64_t binary_point) {
	asm volatile("msr ICC_BPR0_EL1, %0\n\t" : : "r"(binary_point) : "memory");
}

static inline void GIC_SetBinaryPointGroup1(uint64_t binary_point) {
	asm volatile("msr ICC_BPR1_EL1, %0\n\t" : : "r"(binary_point) : "memory");
}

static inline uint32_t GIC_GetBinaryPoint() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_BPR0_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline uint32_t GIC_GetBinaryPointGroup1() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_BPR1_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline void GIC_SetBinaryPointCommonGroup() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_CTLR_EL1\n\t" : "=r"(reg) : : "memory");
	reg &= ~(1 << 0);
	asm volatile("msr ICC_CTLR_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_SetBinaryPointNoCommonGroup() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_CTLR_EL1\n\t" : "=r"(reg) : : "memory");
	reg |= (1 << 0);
	asm volatile("msr ICC_CTLR_EL1, %0\n\t" : : "r"(reg) : "memory");
}

/** \brief Get the status for a given interrupt.
 * \param [in] IRQn The interrupt to get status for.
 * \return 0 - not pending/active, 1 - pending, 2 - active, 3 - pending and active
 */
static inline uint32_t GIC_GetIRQStatus(IRQn_Type IRQn) {
	uint32_t pending, active;

	active = ((HW::GICDistributor->ISACTIVER[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;
	pending = ((HW::GICDistributor->ISPENDR[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;

	return ((active << 1U) | pending);
}

/** \brief Generate a software interrupt using GIC's SGIR register.
 * \param [in] IRQn Software interrupt to be generated.
 * \param [in] target_list List of CPUs the software interrupt should be forwarded to.
 * \param [in] filter_list Filter to be applied to determine interrupt receivers.
 */
static inline void GIC_SendSGI(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list) {
	HW::GICDistributor->SGIR = ((filter_list & 3U) << 24U) | ((target_list & 0xFFUL) << 16U) | (IRQn & 0x0FUL);
}

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
 * \return GICInterface_Type::HPPIR
 */
// static inline uint32_t GIC_GetHighPendingIRQ(void) {
// 	return HW::GICInterface->HPPIR;
// }

/** \brief Set the interrupt group from the GIC's IGROUPR register.
 * \param [in] IRQn The interrupt to be queried.
 * \param [in] group Interrupt group number: 0 - Group 0, 1 - Group 1
 */
static inline void GIC_SetGroup(IRQn_Type IRQn, uint32_t group) {
	uint32_t igroupr = HW::GICDistributor->IGROUPR[IRQn / 32U];
	uint32_t shift = (IRQn % 32U);

	igroupr &= (~(1U << shift));
	igroupr |= ((group & 1U) << shift);

	HW::GICDistributor->IGROUPR[IRQn / 32U] = igroupr;
}
#define GIC_SetSecurity GIC_SetGroup

/** \brief Get the interrupt group from the GIC's IGROUPR register.
 * \param [in] IRQn The interrupt to be queried.
 * \return 0 - Group 0, 1 - Group 1
 */
static inline uint32_t GIC_GetGroup(IRQn_Type IRQn) {
	return (HW::GICDistributor->IGROUPR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}
