#pragma once

namespace mdrivlib
{
enum class Targets {
	stm32mp1_ca7,
	stm32mp1_cm4,
	stm32h7x5,
	stm32f7,
	stm32f4,
	stm32f0,
	simulator,
};

// STM32MP1 CoreA7
#if defined(STM32MP1) && defined(CORE_CA7)
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32mp1_ca7;

// STM32MP1 CoreM4
#elif defined(STM32MP1) && defined(CORE_CM4)
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32mp1_cm4;

// STM32H745 and STM32H755
#elif defined(STM32H745xx) || defined(STM32H755xx)
#ifndef STM32H7x5
#define STM32H7x5
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32h7x5;

// STM32F723
#elif defined(STM32F723xx)
#ifndef STM32F7xx
#define STM32F7xx
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32f7;

// STM32F730
#elif defined(STM32F730xx)
#ifndef STM32F7xx
#define STM32F7xx
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32f7;

// STM32F746
#elif defined(STM32F746xx)
#ifndef STM32F7xx
#define STM32F7xx
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32f7;

// STM32F423
#elif defined(STM32F423xx)
#ifndef STM32F4xx
#define STM32F4xx
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32f4;

// STM32F401xE
#elif defined(STM32F401xE)
#ifndef STM32F4xx
#define STM32F4xx
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32f4;

// STM32F030
#elif defined(STM32F0)
#ifndef STM32F0xx
#define STM32F0xx
#endif
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::stm32f0;

// Simulator
#elif defined(SIMULATOR) || defined(TESTPROJECT)
constexpr bool TargetIsFound = true;
constexpr auto TargetName = Targets::simulator;

#endif
} // namespace mdrivlib

static_assert(mdrivlib::TargetIsFound == true, "No target specified");
