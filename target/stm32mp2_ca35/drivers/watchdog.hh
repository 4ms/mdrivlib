#pragma once
// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
// IWDG pet via the SCP watchdog SMC service. At EL3 there is no secure
// monitor above us, so this is a no-op.
#include "drivers/smc.hh"
#include "drivers/system_reg.hh"
#include <cstdint>

namespace mdrivlib
{

constexpr uint64_t SMC_WD_ID = 0xB200005A;

enum smc_wd_call {
	SMCWD_INIT = 0,
	SMCWD_SET_TIMEOUT = 1,
	SMCWD_ENABLE = 2,
	SMCWD_PET = 3,
	SMCWD_GET_TIMELEFT = 4,
};

inline int watchdog_pet() {
	if (get_current_el() < 3) {
		auto res = smc_call(SMC_WD_ID, SMCWD_PET, 0, 0, 0, 0, 0, 0);
		return res.a0;
	} else
		return 0;
}

} // namespace mdrivlib
