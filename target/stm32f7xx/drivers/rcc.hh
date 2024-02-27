#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

#ifdef STM32F746xx
#include "../../stm32f746/drivers/rcc.hh"
#endif

#ifdef STM32F730xx
#include "../../stm32f730/drivers/rcc.hh"
#endif

#ifdef STM32F723xx
#include "../../stm32f723/drivers/rcc.hh"
#endif
