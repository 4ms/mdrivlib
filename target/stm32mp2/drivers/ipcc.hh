#pragma once
#include "drivers/interrupt.hh"
#include "drivers/ipcc_regs.hh"
#include "drivers/rcc_common.hh"
#include <cstddef>
#include <utility>

// IPCC driver for the STM32MP2. Shared by the Cortex-A35 and Cortex-M33.
// Same public API as target/stm32mp1/drivers/ipcc.hh, so code written against
// MP1 (e.g. drivers/inter_core_comm.hh) compiles unchanged: IPCC_<1> is the
// Cortex-A35 side and IPCC_<2> the Cortex-M33 side of IPCC1, the A35 <-> M33
// mailbox. Verified on STM32MP257 hardware (stm32mp2-baremetal/ipcc), except
// where noted.
//
// Mental model, which the register names do not make obvious:
//   Each channel is one flag per direction. A core SETS its own flag to say
//   "message waiting for you", and CLEARS the other core's flag to say "I've
//   taken your message". So:
//     set_flag<C>()   -> notify the other core        (raises its RX-occupied)
//     clear_flag<C>() -> acknowledge the other core   (raises its TX-free)
//   "RX occupied" therefore fires on the *other* core's flag going high, and
//   "TX free" fires on *this* core's flag going low.
//
// Differences from MP1:
// - 16 channels, and two instances: IPCC_<N> defaults to IPCC1. Use
//   IPCC_<N, IPCC2_BASE> or IPCC2_<N> for IPCC2.
// - Secure vs non-secure interrupts. Three things must agree, or the interrupt
//   silently never arrives:
//     1. the channel's security bit (IPCC CnSECCFGR, set by init_peripheral())
//     2. the interrupt enable bit: SECRXOIE/SECTXFIE (secure) vs RXOIE/TXFIE
//     3. the IRQ: IPCC1_RX_S_IRQn/IPCC1_TX_S_IRQn vs IPCC1_RX_IRQn/IPCC1_TX_IRQn
//   The `Secure` template parameter picks (2) and (3) for every MP1-named
//   function (enable_all_*_isr(), the ISR-registering overloads, HalfDuplexMode)
//   and (1) for init_peripheral(). It defaults to IPCCDefaultSecure (true)
//   because this firmware runs both cores secure + privileged (A35 at EL3, M33
//   secure with the SAU off). The explicitly named *_secure()/*_nonsecure()
//   variants arm one specific line regardless of `Secure`.
// - is_chan_*_isr_enabled() returns true when the channel is unmasked
//   (enabled). MP1's version returned the raw mask bit, i.e. the inverse.
// - The ISR-registering overloads register the handler before unmasking the
//   channel (MP1 unmasked first). The A35 GIC is configured edge-triggered by
//   InterruptControl::enable_irq(), which also clears any pending state; if the
//   line was already high (e.g. the other core sent before we set up), MP1's
//   order would lose that edge for good. Unmasking last makes the edge happen
//   after the GIC is listening.
// - init_peripheral(): one-time setup (clock, reset, RIFSC, channel security).

namespace mdrivlib
{

// Security context of the IPCC channels and interrupt lines used by default.
// Define MDRIVLIB_MP2_NONSECURE to default to the non-secure enables/IRQs.
#if defined(MDRIVLIB_MP2_NONSECURE)
inline constexpr bool IPCCDefaultSecure = false;
#else
inline constexpr bool IPCCDefaultSecure = true;
#endif

// Which IPCC1 processor number (Core<N>) this core is. IPCC1 processor 1 is
// the Cortex-A35 (both A35 cores), processor 2 the Cortex-M33.
#if defined(CORE_CA35)
inline constexpr size_t IPCC1ThisCoreN = 1;
#elif defined(CORE_CM33)
inline constexpr size_t IPCC1ThisCoreN = 2;
#else
inline constexpr size_t IPCC1ThisCoreN = 0;
#endif

// N: IPCC processor number (1 or 2) whose registers this core drives.
// Base: IPCC1_BASE (A35 <-> M33) or IPCC2_BASE.
// Secure: which interrupt enables/lines the MP1-named functions use (see above).
template<size_t N, uint32_t Base = IPCC1_BASE, bool Secure = IPCCDefaultSecure>
struct IPCC_ {
	static_assert(N == 1 || N == 2, "IPCC has Core = 1 and Core = 2 only");
	static_assert(Base == IPCC1_BASE || Base == IPCC2_BASE, "STM32MP2 has IPCC1 and IPCC2 only");

	static constexpr uint32_t NumChannels = 16;
	static constexpr uint32_t AllChannels = (1u << NumChannels) - 1;

	// IRQs that this core receives for this instance, matching `Secure`.
	// The numbers differ per core (see stm32mp257dxx_ca35.h / _cm33.h): e.g.
	// IPCC1_RX_S_IRQn is 205 on the A35 GIC and 173 on the M33 NVIC.
	static constexpr IRQn_Type RXOccIRQn = Base == IPCC1_BASE ? (Secure ? IPCC1_RX_S_IRQn : IPCC1_RX_IRQn) :
																(Secure ? IPCC2_RX_S_IRQn : IPCC2_RX_IRQn);
	static constexpr IRQn_Type TXFreeIRQn = Base == IPCC1_BASE ? (Secure ? IPCC1_TX_S_IRQn : IPCC1_TX_IRQn) :
																 (Secure ? IPCC2_TX_S_IRQn : IPCC2_TX_IRQn);

private:
	template<uint32_t C>
	using ThisCore = typename IPCCRegs::Core<N, Base>::template Chan<C>;
	// The peer's register bank: Core 1 <-> Core 2.
	template<uint32_t C>
	using OtherCore = typename IPCCRegs::Core<3 - N, Base>::template Chan<C>;
	using CoreRegs = IPCCRegs::Core<N, Base>;

	// The ISR-registering overloads route the IRQ to the calling core, so N
	// must be the calling core's side of the mailbox, or the handler never runs.
	// (Only checked for IPCC1, whose processor assignment is verified.)
	static constexpr bool IsThisCoresSide = (Base != IPCC1_BASE) || (N == IPCC1ThisCoreN);

public:
	// Call once, from the A35, before starting the M33 (and before either core
	// enables IPCC interrupts). Clocks and resets the IPCC (clearing all flags
	// and masks), gives the peripheral to the secure world in the RIFSC, marks
	// the channels in chan_mask (bit n = channel n+1) secure (if Secure) and
	// privileged on both processor sides, and clears any stale flags.
	// RCC/RIFSC writes require the A35 to be secure (EL3).
	static void init_peripheral(uint32_t chan_mask = AllChannels) {
		auto ipcc = reinterpret_cast<IPCC_TypeDef *>(Base);
		chan_mask &= AllChannels;

		if constexpr (Base == IPCC1_BASE) {
			RCC_Enable::IPCC1_::set();
			RCC_Reset::IPCC1_::set();
			RCC_Reset::IPCC1_::clear();
		} else {
			RCC_Enable::IPCC2_::set();
			RCC_Reset::IPCC2_::set();
			RCC_Reset::IPCC2_::clear();
		}

		if constexpr (Secure) {
			// RIFSC RISUP peripheral id (stm32mp2xx_hal_rif.h RIF_PERIPH_IPCCx):
			// mark the peripheral secure so our secure masters own it
			constexpr uint32_t RifscId = Base == IPCC1_BASE ? 144 : 145;
			RISC->SECCFGR[RifscId / 32] = RISC->SECCFGR[RifscId / 32] | (1u << (RifscId % 32));

			ipcc->C1SECCFGR = ipcc->C1SECCFGR | chan_mask;
			ipcc->C2SECCFGR = ipcc->C2SECCFGR | chan_mask;
		} else {
			ipcc->C1SECCFGR = ipcc->C1SECCFGR & ~chan_mask;
			ipcc->C2SECCFGR = ipcc->C2SECCFGR & ~chan_mask;
		}
		ipcc->C1PRIVCFGR = ipcc->C1PRIVCFGR | chan_mask;
		ipcc->C2PRIVCFGR = ipcc->C2PRIVCFGR | chan_mask;

		// Start from a known state: no stale flags from a previous run.
		// The low half of CnSCR is "clear the other processor's flag".
		ipcc->C1SCR = chan_mask;
		ipcc->C2SCR = chan_mask;
	}

	// Set/Clear the IPCC Flag:
	// Note: Clearing acts upon the other core's flag
	// and reading the rx occupied flag requires reading the other Core's flags
	template<uint32_t C>
	static void set_flag() {
		ThisCore<C>::ChangeFlag::set();
	}
	template<uint32_t C>
	static void clear_flag() {
		ThisCore<C>::ChangeFlag::clear();
	}
	template<uint32_t C>
	static bool is_tx_free() {
		return ThisCore<C>::FlagStatus::read() == 0;
	}
	template<uint32_t C>
	static bool is_rx_occupied() {
		return OtherCore<C>::FlagStatus::read() != 0;
	}
	template<uint32_t C>
	static bool is_other_rx_occupied() {
		return ThisCore<C>::FlagStatus::read() != 0;
	}

	// TXFree ISR:
	template<uint32_t C>
	static void enable_chan_txfree_isr() {
		ThisCore<C>::FreeISRMasked::clear(); // enable = clear a mask bit (unmask)
	}
	// Arms this core's TX-free line (per `Secure`), registers and starts the
	// handler on this core, then unmasks channel C.
	template<uint32_t C>
	static void enable_chan_txfree_isr(uint32_t pri, uint32_t subpri, auto &&callback) {
		static_assert(IsThisCoresSide, "IPCC1_<N>: N must be this core's side (A35 = 1, M33 = 2) to register an ISR");
		enable_all_txfree_isr();
		InterruptManager::register_and_start_isr(TXFreeIRQn, pri, subpri, std::forward<decltype(callback)>(callback));
		enable_chan_txfree_isr<C>();
	}
	template<uint32_t C>
	static void disable_chan_txfree_isr() {
		ThisCore<C>::FreeISRMasked::set();
	}
	// Per-core enable for all channels, on the line selected by `Secure`
	static void enable_all_txfree_isr() {
		if constexpr (Secure)
			enable_all_txfree_isr_secure();
		else
			enable_all_txfree_isr_nonsecure();
	}
	static void disable_all_txfree_isr() {
		if constexpr (Secure)
			disable_all_txfree_isr_secure();
		else
			disable_all_txfree_isr_nonsecure();
	}
	// MP1 spelling
	static void disble_all_txfree_isr() {
		disable_all_txfree_isr();
	}
	// A channel marked secure in CnSECCFGR only ever asserts the secure line,
	// so a secure channel needs the _secure enable (and vice versa).
	static void enable_all_txfree_isr_secure() {
		CoreRegs::SecTXFreeISREnable::set();
	}
	static void disable_all_txfree_isr_secure() {
		CoreRegs::SecTXFreeISREnable::clear();
	}
	static void enable_all_txfree_isr_nonsecure() {
		CoreRegs::TXFreeISREnable::set();
	}
	static void disable_all_txfree_isr_nonsecure() {
		CoreRegs::TXFreeISREnable::clear();
	}
	// Note: the mask bit is 1 == masked, so "enabled" is the cleared bit.
	// (MP1's version returned the raw mask bit.)
	template<uint32_t C>
	static bool is_chan_txfree_isr_enabled() {
		return ThisCore<C>::FreeISRMasked::read() == 0;
	}

	// RXOccupied:
	// Note: RXOcc ISR triggers on the other core's flag going high
	template<uint32_t C>
	static void enable_chan_rxocc_isr() {
		ThisCore<C>::OccISRMasked::clear(); // enable = clear a mask bit (unmask)
	}
	// Arms this core's RX-occupied line (per `Secure`), registers and starts
	// the handler on this core, then unmasks channel C.
	// The handler must acknowledge (clear_flag<C>()) or disable the channel, or
	// it re-fires as soon as it returns.
	template<uint32_t C>
	static void enable_chan_rxocc_isr(uint32_t pri, uint32_t subpri, auto &&callback) {
		static_assert(IsThisCoresSide, "IPCC1_<N>: N must be this core's side (A35 = 1, M33 = 2) to register an ISR");
		enable_all_rxocc_isr();
		InterruptManager::register_and_start_isr(RXOccIRQn, pri, subpri, std::forward<decltype(callback)>(callback));
		enable_chan_rxocc_isr<C>();
	}
	template<uint32_t C>
	static void disable_chan_rxocc_isr() {
		ThisCore<C>::OccISRMasked::set(); // disable = set a mask bit
	}
	// Per-core enable for all channels, on the line selected by `Secure`
	static void enable_all_rxocc_isr() {
		if constexpr (Secure)
			enable_all_rxocc_isr_secure();
		else
			enable_all_rxocc_isr_nonsecure();
	}
	static void disable_all_rxocc_isr() {
		if constexpr (Secure)
			disable_all_rxocc_isr_secure();
		else
			disable_all_rxocc_isr_nonsecure();
	}
	// MP1 spelling
	static void disble_all_rxocc_isr() {
		disable_all_rxocc_isr();
	}
	static void enable_all_rxocc_isr_secure() {
		CoreRegs::SecRXOccISREnable::set();
	}
	static void disable_all_rxocc_isr_secure() {
		CoreRegs::SecRXOccISREnable::clear(); // disable = clear an enable bit
	}
	static void enable_all_rxocc_isr_nonsecure() {
		CoreRegs::RXOccISREnable::set();
	}
	static void disable_all_rxocc_isr_nonsecure() {
		CoreRegs::RXOccISREnable::clear();
	}
	template<uint32_t C>
	static bool is_chan_rxocc_isr_enabled() {
		return ThisCore<C>::OccISRMasked::read() == 0;
	}

	// Half Duplex mode:
	// One shared message buffer per channel, ownership passed back and forth.
	// Core 1 owns it while its flag is low (TX free); Core 2 owns it while
	// Core 1's flag is high (RX occupied). Core 1 hands over by setting its
	// flag, Core 2 hands back by clearing it.
	template<uint32_t C>
	struct HalfDuplexMode {
		static auto is_my_turn() {
			if constexpr (N == 1)
				return is_tx_free<C>();
			else
				return is_rx_occupied<C>();
		}

		static void enable_chan_isr(uint32_t pri, uint32_t subpri, auto &&callback) {
			if constexpr (N == 1)
				enable_chan_txfree_isr<C>(pri, subpri, std::forward<decltype(callback)>(callback));
			else
				enable_chan_rxocc_isr<C>(pri, subpri, std::forward<decltype(callback)>(callback));
		}

		static auto notify_other() {
			if constexpr (N == 1)
				set_flag<C>();
			else
				clear_flag<C>();
		}

		static auto enable_chan_isr() {
			if constexpr (N == 1)
				enable_chan_txfree_isr<C>();
			else
				enable_chan_rxocc_isr<C>();
		}

		static auto disable_chan_isr() {
			if constexpr (N == 1)
				disable_chan_txfree_isr<C>();
			else
				disable_chan_rxocc_isr<C>();
		}

		static bool is_chan_isr_enabled() {
			if constexpr (N == 1)
				return is_chan_txfree_isr_enabled<C>();
			else
				return is_chan_rxocc_isr_enabled<C>();
		}
	};
};

// Explicit-instance spellings. IPCC1 is the A35 <-> M33 mailbox.
// IPCC2 (SmartRun domain) is untested; check RM0457 for which processors are
// its Core 1 and Core 2 before using it.
template<size_t N, bool Secure = IPCCDefaultSecure>
using IPCC1_ = IPCC_<N, IPCC1_BASE, Secure>;
template<size_t N, bool Secure = IPCCDefaultSecure>
using IPCC2_ = IPCC_<N, IPCC2_BASE, Secure>;

namespace RCC_Enable
{
// MP1 name, used by drivers/inter_core_comm.hh (MP1 has a single IPCC). On MP2
// the A35 <-> M33 mailbox is IPCC1. Redeclaring the same alias is legal, so this
// stays harmless if rcc_common.hh ever adds it.
using IPCC_ = IPCC1_;
} // namespace RCC_Enable

} // namespace mdrivlib
