#pragma once
#include "drivers/interrupt.hh"
#include "drivers/rcc_common.hh"
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"
#include <functional>

// Hardware semaphore (HSEM) driver for the STM32MP2. Shared by the Cortex-A35
// and Cortex-M33. Same public API as target/stm32mp1/drivers/hsem.hh. Ported
// from stm32mp2-baremetal/hw_semaphore, verified on STM32MP257 hardware (the
// two-step lock(procID)/unlock(procID) and the secure interrupt path; the
// one-step lock() was not exercised there).
//
// Differences from MP1:
// - 16 semaphores, not 32.
// - Three core IDs, not two: CA35 = 1, CM33 = 2, CM0PLUS = 3. Both A35 cores
//   share core ID 1 -- HSEM cannot tell them apart (same as the two A7 cores on
//   MP1), so use a distinct processID per A35 core to arbitrate between them.
//   The device header hardcodes HSEM_CR_COREID_CURRENT per core, so the same
//   code compiles correctly for both the A35 and M33 builds.
// - MP2 adds SEC and PRIV bits to the lock word, and a separate bank of secure
//   interrupt registers (SCnIER/ICR/ISR/MISR) next to the non-secure ones
//   (CnIER/...). With HSemSecure (the default: this firmware runs both cores
//   secure + privileged), every lock carries SEC|PRIV and the secure bank and
//   HSEM_S_IRQn are used. A secure semaphore (HSEM->SECCFGR bit set, see
//   HWSemaphoreGlobalBase::init_peripheral()) only signals on the secure line.
// - The lock read-back check must ignore CFEN (bit 30), which the hardware sets
//   to report CID filtering -- it is never part of what we wrote.
// - Interrupts: both A35 cores share one SCnIER bank and one GIC interrupt line
//   (they share core ID 1), so only one A35 core can service HSEM interrupts.
//   That core must service the semaphores the other A35 core waits on, and
//   hand over via shared memory.
// - HWSemaphoreGlobalBase::enable_all_handlers() takes the IRQ as before, but
//   defaults to HSemIRQn (MP1 code passes HSEM_IT1_IRQn/HSEM_IT2_IRQn, which
//   don't exist on MP2: use HSEM_S_IRQn/HSEM_IRQn, or just HSemIRQn).
// - HWSemaphoreGlobalBase::init_peripheral(): one-time setup.

namespace mdrivlib
{

// Security context of the semaphores and interrupt line used by this driver.
// Define MDRIVLIB_MP2_NONSECURE to use non-secure lock words, the non-secure
// interrupt bank and HSEM_IRQn. PRIV is always set: firmware runs privileged.
#if defined(MDRIVLIB_MP2_NONSECURE)
inline constexpr bool HSemSecure = false;
#else
inline constexpr bool HSemSecure = true;
#endif

inline constexpr uint32_t HSemNumSemaphores = HSEM_SEMID_MAX + 1;

// This core's ID as encoded in the lock word: 1 = CA35, 2 = CM33, 3 = CM0PLUS.
inline constexpr uint32_t HSemCurrentCoreId = HSEM_CR_COREID_CURRENT >> HSEM_CR_COREID_Pos;

// Lock-word attribute bits. The hardware records the security/privilege of the
// locking access; we write them too so the read-back compare is exact.
inline constexpr uint32_t HSemLockAttr = (HSemSecure ? HSEM_R_SEC : 0) | HSEM_R_PRIV;

// The IRQ this core receives HSEM interrupts on, matching HSemSecure.
// HSEM_S_IRQn is 232 on the A35 GIC and 200 on the M33 NVIC.
inline constexpr IRQn_Type HSemIRQn = HSemSecure ? HSEM_S_IRQn : HSEM_IRQn;

enum class HWSemaphoreFlag {
	LockFailed = 0,
	LockedOk = 1,
	// AlreadySet, SameCoreAlreadySet, OtherCoreAlreadySet
};

// The HSEM registers are Device memory, but the data these semaphores protect is
// not: it lives in shared RAM mapped Normal (Non-cacheable). The Arm memory
// model (ARMv7-A on MP1; ARMv8-A on the A35 and ARMv8-M on the M33 here) lets
// Normal accesses be buffered and reordered around Device accesses, so without
// a barrier the other core can observe the semaphore released before the data
// it guards has landed, and read it torn or stale.
//
// A DMB on each side turns lock()/unlock() into a proper acquire/release pair,
// so callers can wrap plain (non-atomic) shared data and be correct. Users that
// carry their own barriers -- seqlocks, the SPSC queues -- are unaffected.
//
// This mattered less when shared RAM was mapped Strongly-Ordered, which forbids
// that reordering; don't remove these if that mapping ever comes back, because
// the guarantee would then be coming from the wrong place.
template<uint32_t SemaphoreID>
struct HWSemaphore {
	static_assert(SemaphoreID <= HSEM_SEMID_MAX, "STM32MP2 HSEM has semaphores 0 to 15");

	HWSemaphore() = delete;

	// One-step ("fast") lock: reading RLR attempts the take, and the value read
	// back tells you whether you now own it (with processID 0).
	// CAUTION: this cannot arbitrate between the A35 cores since they both
	// have core ID 1. Use the two-step lock(processID) with a distinct ID per
	// participant unless every contender has its own core ID.
	static HWSemaphoreFlag lock() {
		constexpr uint32_t want = HSEM_RLR_LOCK | HSemLockAttr | HSEM_CR_COREID_CURRENT;
		if ((HSEM->RLR[SemaphoreID] & ~HSEM_RLR_CFEN) != want)
			return HWSemaphoreFlag::LockFailed;

		acquire_barrier();
		return HWSemaphoreFlag::LockedOk;
	}

	// Two-step lock: write the lock word, then read it back to confirm we won.
	static HWSemaphoreFlag lock(uint32_t processID) {
		const uint32_t want = HSEM_R_LOCK | HSemLockAttr | HSEM_CR_COREID_CURRENT | processID;
		HSEM->R[SemaphoreID] = want;
		if ((HSEM->R[SemaphoreID] & ~HSEM_R_CFEN) != want)
			return HWSemaphoreFlag::LockFailed;

		acquire_barrier();
		return HWSemaphoreFlag::LockedOk;
	}

	// Releasing = writing the same core/process ID back with the LOCK bit clear.
	// A write from a core/process that does not hold it is ignored by the hardware.
	static void unlock() {
		release_barrier();
		HSEM->R[SemaphoreID] = HSemLockAttr | HSEM_CR_COREID_CURRENT;
	}

	static void unlock(uint32_t processID) {
		release_barrier();
		HSEM->R[SemaphoreID] = HSemLockAttr | HSEM_CR_COREID_CURRENT | processID;
	}

	static void unlock_nonrecursive(uint32_t processID) {
		disable_channel_ISR();
		unlock(processID);
		enable_channel_ISR();
	}

	static bool is_locked() {
		return HSEM->R[SemaphoreID] & HSEM_R_LOCK;
	}

	// --- interrupts (fire when the semaphore is *released*) ------------------
	static void enable_channel_ISR() {
		ier() = ier() | (1u << SemaphoreID);
	}

	static void disable_channel_ISR() {
		ier() = ier() & ~(1u << SemaphoreID);
	}

	static void clear_ISR() {
		icr() = 1u << SemaphoreID;
	}

	// Nothing the caller does with the guarded data may be reordered before the
	// lock was seen, or after the unlock becomes visible. Also stops the
	// compiler moving the accesses across.
	static void acquire_barrier() {
		barrier();
	}

	static void release_barrier() {
		barrier();
	}

	// aka: is_status_after_masking_pending()
	static bool is_ISR_triggered_and_enabled() {
		return misr() & (1u << SemaphoreID);
	}

private:
	static void barrier() {
#if defined(CORE_CA35)
		asm volatile("dmb sy" ::: "memory");
#elif defined(CORE_CM33) || defined(CORE_CM0)
		__DMB();
#else
#error "STM32MP2 HWSemaphore: define CORE_CA35 or CORE_CM33"
#endif
	}

	// Interrupt register bank for this core, selected at compile time by this
	// core's ID and HSemSecure.
	static volatile uint32_t &ier() {
		if constexpr (HSemSecure) {
			if constexpr (HSemCurrentCoreId == HSEM_CPU1_COREID)
				return HSEM->SC1IER;
			else if constexpr (HSemCurrentCoreId == HSEM_CPU2_COREID)
				return HSEM->SC2IER;
			else
				return HSEM->SC3IER;
		} else {
			if constexpr (HSemCurrentCoreId == HSEM_CPU1_COREID)
				return HSEM->C1IER;
			else if constexpr (HSemCurrentCoreId == HSEM_CPU2_COREID)
				return HSEM->C2IER;
			else
				return HSEM->C3IER;
		}
	}

	static volatile uint32_t &icr() {
		if constexpr (HSemSecure) {
			if constexpr (HSemCurrentCoreId == HSEM_CPU1_COREID)
				return HSEM->SC1ICR;
			else if constexpr (HSemCurrentCoreId == HSEM_CPU2_COREID)
				return HSEM->SC2ICR;
			else
				return HSEM->SC3ICR;
		} else {
			if constexpr (HSemCurrentCoreId == HSEM_CPU1_COREID)
				return HSEM->C1ICR;
			else if constexpr (HSemCurrentCoreId == HSEM_CPU2_COREID)
				return HSEM->C2ICR;
			else
				return HSEM->C3ICR;
		}
	}

	static volatile uint32_t &misr() {
		if constexpr (HSemSecure) {
			if constexpr (HSemCurrentCoreId == HSEM_CPU1_COREID)
				return HSEM->SC1MISR;
			else if constexpr (HSemCurrentCoreId == HSEM_CPU2_COREID)
				return HSEM->SC2MISR;
			else
				return HSEM->SC3MISR;
		} else {
			if constexpr (HSemCurrentCoreId == HSEM_CPU1_COREID)
				return HSEM->C1MISR;
			else if constexpr (HSemCurrentCoreId == HSEM_CPU2_COREID)
				return HSEM->C2MISR;
			else
				return HSEM->C3MISR;
		}
	}
};

struct HWSemaphoreGlobalBase {
	using CallbackT = std::function<void(void)>;

	static constexpr uint32_t AllSemaphores = (1u << HSemNumSemaphores) - 1;

	// Call once, from the A35, before starting the M33 (and before any core
	// locks a semaphore: the reset releases every lock). Clocks and resets the
	// HSEM, gives the peripheral to the secure world in the RIFSC, and marks the
	// semaphores in sem_mask (bit n = semaphore n) secure (if HSemSecure) and
	// privileged. RCC/RIFSC writes require the A35 to be secure (EL3).
	static void init_peripheral(uint32_t sem_mask = AllSemaphores) {
		sem_mask &= AllSemaphores;

		RCC_Enable::HSEM_::set();

		// Reset to clear all locks
		RCC_Reset::HSEM_::set();
		RCC_Reset::HSEM_::clear();

		if constexpr (HSemSecure) {
			// RIFSC RISUP peripheral id (stm32mp2xx_hal_rif.h RIF_PERIPH_HSEM):
			// mark the peripheral secure so our secure masters own it
			constexpr uint32_t RifscId_HSEM = 146;
			RISC->SECCFGR[RifscId_HSEM / 32] = RISC->SECCFGR[RifscId_HSEM / 32] | (1u << (RifscId_HSEM % 32));

			HSEM->SECCFGR = HSEM->SECCFGR | sem_mask;
		} else {
			HSEM->SECCFGR = HSEM->SECCFGR & ~sem_mask;
		}
		HSEM->PRIVCFGR = HSEM->PRIVCFGR | sem_mask;
	}

	template<unsigned SemaphoreID>
	static void register_channel_ISR(CallbackT &&func) {
		static_assert(SemaphoreID < HSemNumSemaphores, "STM32MP2 HSEM has semaphores 0 to 15");
		funcs[SemaphoreID] = std::move(func);
	}

	static void enable_all_handlers(IRQn_Type irqn = HSemIRQn) {
		InterruptManager::register_and_start_isr(irqn, 0, 0, [&]() {
			handle_isr<0>();
			handle_isr<1>();
			handle_isr<2>();
			handle_isr<3>();
			handle_isr<4>();
			handle_isr<5>();
			handle_isr<6>();
			handle_isr<7>();
			handle_isr<8>();
			handle_isr<9>();
			handle_isr<10>();
			handle_isr<11>();
			handle_isr<12>();
			handle_isr<13>();
			handle_isr<14>();
			handle_isr<15>();
		});
	}

	template<unsigned SemaphoreID>
	static void handle_isr() {
		if (HWSemaphore<SemaphoreID>::is_ISR_triggered_and_enabled()) {
			if (funcs[SemaphoreID])
				funcs[SemaphoreID]();
			HWSemaphore<SemaphoreID>::clear_ISR();
			return;
		}
	}

	static inline CallbackT funcs[HSemNumSemaphores];
};
} // namespace mdrivlib
