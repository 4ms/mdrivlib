#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{

namespace stm32h7x5
{

using SYSCFG_FMP = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, PMCR), 0, 7>;
}
} // namespace mdrivlib
