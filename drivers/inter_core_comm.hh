#pragma once
#include "drivers/ipcc.hh"
#include "drivers/rcc.hh"

// #include "debug.hh"
// #include "printf.h"

namespace mdrivlib
{

enum class ICCCoreType { Initiator = 1, Responder = 2 };

template<ICCCoreType Core, typename MessageT>
class InterCoreComm {
	static constexpr auto CoreN = static_cast<uint32_t>(Core); //1 or 2
	static constexpr uint32_t Chan = 1;
	using IPCCHalfDuplex = typename mdrivlib::IPCC_<(uint32_t)Core>::template HalfDuplexMode<Chan>;

	//Access is protected via IPCC hardware Flag, so it does not need to be atomic
	MessageT &shared_message_;

	bool was_my_turn = false;

public:
	InterCoreComm(MessageT &shared_message)
		: shared_message_{shared_message} {
		mdrivlib::RCC_Enable::IPCC_::set();
	}

	[[nodiscard]] bool send_message(const MessageT &msg) {
		// DebugN<CoreN + 1>::Pin::high();

		if (!IPCCHalfDuplex::is_my_turn()) {
			pr_dbg("%d: send aborted: not my turn\n", Core);
			return false;
		}

		shared_message_ = msg;
		was_my_turn = false;

		__DMB();
		// pr_dbg("[%d] %d: sending %d\n", HAL_GetTick(), Core, msg.message_type);
		IPCCHalfDuplex::notify_other();

		// DebugN<CoreN + 1>::Pin::low();
		return true;
	}

	[[nodiscard]] MessageT get_new_message() {
		// Process messages once
		auto is_my_turn = IPCCHalfDuplex::is_my_turn();

		MessageT msg{};

		if (is_my_turn && !was_my_turn) {
			// DebugN<CoreN - 1>::Pin::high();

			msg = shared_message_;

			// pr_dbg("[%d] %d: got msg %d\n", HAL_GetTick(), Core, msg.message_type);
			shared_message_ = MessageT{};

			// DebugN<CoreN - 1>::Pin::low();
		} else if (!is_my_turn) {
			pr_trace("%d: get_new_message: not my turn\n", Core);
		} else if (was_my_turn) {
			pr_dbg("%d: get_new_message: already got message\n", Core);
		}
		was_my_turn = is_my_turn;

		return msg;
	}

	template<typename... Ts>
	static void pr_dbg(Ts... args) {
		// printf_(args...);
	}
	template<typename... Ts>
	static void pr_trace(Ts... args) {
		// printf(args...);
	}
};

} // namespace mdrivlib
