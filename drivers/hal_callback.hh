#pragma once

#include "stm32xx.h"
#include <functional>

namespace mdrivlib
{

// Note: this is only used by PCA9685 DMA driver. Is it worth keeping?
// Using this locks us into ST's I2C HAL, and an extra layer of re-direction
// in response to an IRQ.
enum class HALCallbackID {
	I2C_MemTxCplt,
	MAX,
};
const int kMaxHALCallbackIDs = static_cast<uint32_t>(HALCallbackID::MAX);

class HALCallbackManager {
public:
	struct HALCBBase {
		virtual void halcb() = 0;
	};
	using HALCBType = HALCBBase *;
	static inline void registerHALCB(HALCallbackID cbnum, HALCBType &&cb_obj) {
		HALCBs[get_cb_int(cbnum)] = std::move(cb_obj);
	}
	static inline void callHALCB(HALCallbackID cbnum) {
		HALCBs[get_cb_int(cbnum)]->halcb();
	}
	static uint8_t constexpr get_cb_int(HALCallbackID cbnum) {
		return static_cast<uint8_t>(cbnum);
	}

private:
	static inline HALCBType HALCBs[kMaxHALCallbackIDs];
};

struct HALCallback : public HALCallbackManager::HALCBBase {
	using Func = std::function<void(void)>;

	HALCallback(HALCallbackID cbnum, const Func &&func)
		: func_(std::move(func)) {
		HALCallbackManager::registerHALCB(cbnum, this);
	}
	void halcb() override {
		func_();
	}

private:
	const std::function<void(void)> func_;
};
} // namespace mdrivlib
