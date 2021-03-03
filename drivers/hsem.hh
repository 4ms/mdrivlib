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

struct HWSemaphore {
	enum Results { LockedOk, LockFailed, SameCoreAlreadyLocked, OtherCoreAlreadyLocked };
	enum SetResults { SetOk, AlreadySet, SameCoreAlreadySet, OtherCoreAlreadySet };

	template<uint32_t SemaphoreID>
	static SetResults set() {
		// One-step lock:
		// Read Lock Semaphore with HSEM_CR_COREID_CURRENT
		// If COREID matches HSEM_CR_COREID_CURRENT:
		//     if PROCID = 0, then return LockedOK
		// 	   else return SameCoreAlreadyLocked
		// else: return OtherCoreAlreadyLocked
		return (HSEM->RLR[SemaphoreID] != (HSEM_R_LOCK | HSEM_CR_COREID_CURRENT)) ? AlreadySet : SetOk;
	}

	template<uint32_t SemaphoreID>
	static Results set(uint32_t processID) {
		// Two-step lock:
		HSEM->R[SemaphoreID] = HSEM_R_LOCK | HSEM_CR_COREID_CURRENT | processID;
		return (HSEM->R[SemaphoreID] != (HSEM_R_LOCK | HSEM_CR_COREID_CURRENT | processID)) ? LockFailed : LockedOk;
	}

	template<uint32_t SemaphoreID>
	static void clear() {
		HSEM->R[SemaphoreID] = HSEM_CR_COREID_CURRENT;
	}

	template<uint32_t SemaphoreID>
	static void clear(uint32_t processID) {
		HSEM->R[SemaphoreID] = HSEM_CR_COREID_CURRENT | processID;
	}

	template<uint32_t SemaphoreID>
	static bool is_locked() {
		return HSEM->R[SemaphoreID] & HSEM_R_LOCK;
	}
};
