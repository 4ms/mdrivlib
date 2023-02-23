#pragma once
#include "drivers/interrupt.hh"
#include "drivers/ipcc_regs.hh"

namespace mdrivlib
{
template<size_t N>
struct IPCC_ {
	static_assert(N == 1 || N == 2, "IPCC has Core = 1 and Core = 2 only");

	// Set/Clear the IPCC Flag:
	template<uint32_t C>
	static void clear_flag() {
		IPCCRegs::Core<N>::template Chan<C>::ChangeFlag::clear();
	}
	template<uint32_t C>
	static void set_flag() {
		IPCCRegs::Core<N>::template Chan<C>::ChangeFlag::set();
	}

	// TXFree:
	template<uint32_t C>
	static void enable_chan_txfree_isr() {
		IPCCRegs::Core<N>::template Chan<C>::FreeISRMasked::clear(); //enable = clear a mask bit (unmask)
	}
	template<uint32_t C>
	static void enable_chan_txfree_isr(uint32_t pri, uint32_t subpri, auto &&callback) {
		IPCCRegs::Core<N>::TXFreeISREnable::set();
		enable_chan_txfree_isr<C>();
		InterruptManager::register_and_start_isr(N == 1 ? IPCC_TX0_IRQn : IPCC_TX1_IRQn, pri, subpri, callback);
	}
	template<uint32_t C>
	static void disable_chan_txfree_isr() {
		IPCCRegs::Core<N>::template Chan<C>::FreeISRMasked::set();
	}
	static void disble_all_txfree_isr() {
		IPCCRegs::Core<N>::TXFreeISREnable::clear();
	}
	template<uint32_t C>
	static bool is_chan_txfree_isr_enabled() {
		return IPCCRegs::Core<N>::template Chan<C>::FreeISRMasked::read();
	}
	template<uint32_t C>
	static bool is_tx_free() {
		return IPCCRegs::Core<N>::template Chan<C>::FlagStatus::read() == 0;
	}

	// RXOccupied:
	template<uint32_t C>
	static void enable_chan_rxocc_isr() {
		IPCCRegs::Core<N>::template Chan<C>::OccISRMasked::clear(); //enable = clear a mask bit (unmask)
	}
	template<uint32_t C>
	static void enable_chan_rxocc_isr(uint32_t pri, uint32_t subpri, auto &&callback) {
		IPCCRegs::Core<N>::RXOccISREnable::set();
		enable_chan_rxocc_isr<C>();
		InterruptManager::register_and_start_isr(N == 1 ? IPCC_RX0_IRQn : IPCC_RX1_IRQn, pri, subpri, callback);
	}
	template<uint32_t C>
	static void disable_chan_rxocc_isr() {
		IPCCRegs::Core<N>::template Chan<C>::OccISRMasked::set(); //disable = set a mask bit
	}
	static void disble_all_rxocc_isr() {
		IPCCRegs::Core<N>::RXOccISREnable::clear(); //disable = clear an enable bit
	}
	template<uint32_t C>
	static bool is_chan_rxocc_isr_enabled() {
		return IPCCRegs::Core<N>::template Chan<C>::OccISRMasked::read();
	}
	template<uint32_t C>
	static bool is_rx_occupied() {
		return IPCCRegs::Core<N>::template Chan<C>::FlagStatus::read() != 0;
	}
	template<uint32_t C>
	static bool is_other_rx_occupied() {
		return IPCCRegs::Core<3 - N>::template Chan<C>::FlagStatus::read() != 0;
	}

	//Half Duplex mode:
	template<uint32_t C>
	struct HalfDuplexMode {
		static auto is_my_turn() {
			if constexpr (N == 1)
				return is_tx_free<C>();
			else
				// Core<1>'s flag holds the state of the halfduplex communication
				return is_other_rx_occupied<C>();
		}

		static void enable_chan_isr(uint32_t pri, uint32_t subpri, auto &&callback) {
			if constexpr (N == 1)
				enable_chan_txfree_isr<C>(pri, subpri, callback);
			else
				enable_chan_rxocc_isr<C>(pri, subpri, callback);
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

} // namespace mdrivlib
