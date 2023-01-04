#pragma once
#include "register_access.hh"
#include "stm32xx.h"

#ifdef STM32F746xx
#include "../../stm32f746/drivers/rcc.hh"
#endif

#ifdef STM32F723xx
#include "../../stm32f723/drivers/rcc.hh"
#endif
