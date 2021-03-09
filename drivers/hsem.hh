#pragma once
#include "arch.hh"
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{

namespace stm32h7x5
{
struct HSEM_ {
	template<uint32_t Mask>
	using IER = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, IER), Mask>;

	template<uint32_t Mask>
	using ICR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, ICR), Mask>;

	template<uint32_t Mask>
	using ISR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, ISR), Mask>;

	template<uint32_t Mask>
	using MISR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, MISR), Mask>;
};
} // namespace stm32h7x5
} // namespace mdrivlib

enum class HWSemaphoreFlag {
	LockFailed = 0,
	LockedOk = 1,
	// AlreadySet, SameCoreAlreadySet, OtherCoreAlreadySet
};
template<uint32_t SemaphoreID>
struct HWSemaphore {
	HWSemaphore() = delete;

	static HWSemaphoreFlag lock() {
		// One-step lock:
		// Read Lock Semaphore with HSEM_CR_COREID_CURRENT
		// If COREID matches HSEM_CR_COREID_CURRENT:
		//     if PROCID = 0, then return LockedOk
		// 	   else return SetOk (SameCoreAlreadyLocked)
		// else: return LockFailed
		return (HSEM->RLR[SemaphoreID] == (HSEM_R_LOCK | HSEM_CR_COREID_CURRENT)) ? HWSemaphoreFlag::LockedOk
																				  : HWSemaphoreFlag::LockFailed;
	}

	static HWSemaphoreFlag lock(uint32_t processID) {
		// Two-step lock:
		HSEM->R[SemaphoreID] = HSEM_R_LOCK | HSEM_CR_COREID_CURRENT | processID;
		return (HSEM->R[SemaphoreID] == (HSEM_R_LOCK | HSEM_CR_COREID_CURRENT | processID))
				 ? HWSemaphoreFlag::LockedOk
				 : HWSemaphoreFlag::LockFailed;
	}

	static void unlock() {
		HSEM->R[SemaphoreID] = HSEM_CR_COREID_CURRENT;
	}

	static void unlock(uint32_t processID) {
		HSEM->R[SemaphoreID] = HSEM_CR_COREID_CURRENT | processID;
	}

	static bool is_locked() {
		return HSEM->R[SemaphoreID] & HSEM_R_LOCK;
	}

	static void enable_ISR() {
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU1)
			HSEM->C1IER = HSEM->C1IER | (1 << SemaphoreID);
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU2)
			HSEM->C2IER = HSEM->C2IER | (1 << SemaphoreID);
	}

	static void disable_ISR() {
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU1)
			HSEM->C1IER = HSEM->C1IER & ~(1 << SemaphoreID);
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU2)
			HSEM->C2IER = HSEM->C2IER & ~(1 << SemaphoreID);
	}

	static void clear_ISR() {
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU1)
			HSEM->C1ICR = 1 << SemaphoreID;
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU2)
			HSEM->C2ICR = 1 << SemaphoreID;
	}

	// aka: is_status_after_masking_pending()
	static bool is_ISR_triggered_and_enabled() {
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU1)
			return HSEM->C1MISR & (1 << SemaphoreID);
		if constexpr (HSEM_CR_COREID_CURRENT == HSEM_CR_COREID_CPU2)
			return HSEM->C2MISR & (1 << SemaphoreID);
	}
};
