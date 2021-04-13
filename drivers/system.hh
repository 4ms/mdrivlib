#pragma once
#include "arch.hh"

#if defined(STM32F7xx)
#include "target/stm32f7xx/system.hh"
#endif

#if defined(STM32H7x5)
#include "target/stm32h7x5/system.hh"
#endif

#if defined(STM32MP1)
#include "target/stm32mp1/system.hh"
#endif
