#pragma once
#include "arch.hh"

#if defined(STM32H7x5)
#include "target/stm32h7x5/spi_registers.hh"
#endif

#if defined(STM32MP1)
#include "target/stm32mp1/spi_registers.hh"
#endif
