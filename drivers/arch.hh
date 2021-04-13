#pragma once

namespace mdrivlib
{
enum class SupportedTargets {
	stm32mp1,
	stm32h7x5,
	stm32f7,
	stm32f4,
};
}

//
// STM32MP1
//
#if defined(STM32MP1)
namespace mdrivlib
{
namespace stm32mp1
{
constexpr bool TARGET_FOUND = true;
constexpr auto TYPE = SupportedTargets::stm32mp1;
} // namespace stm32mp1
} // namespace mdrivlib
namespace target = mdrivlib::stm32mp1;
#endif

//
// STM32H745 and STM32H755
//
#if defined(STM32H745xx) || defined(STM32H755xx)
#ifndef STM32H7x5
#define STM32H7x5
#endif
namespace mdrivlib
{
namespace stm32h7x5
{
constexpr bool TARGET_FOUND = true;
constexpr auto TYPE = SupportedTargets::stm32mp1;
} // namespace stm32h7x5
} // namespace mdrivlib
namespace target = mdrivlib::stm32h7x5;
#endif

//
// STM32F746
//
#if defined(STM32F746xx)
namespace mdrivlib
{
namespace stm32f7xx
{
constexpr bool TARGET_FOUND = true;
constexpr auto TYPE = SupportedTargets::stm32mp1;
} // namespace stm32f7xx
} // namespace mdrivlib
namespace target = mdrivlib::stm32f7xx;
#endif

static_assert(target::TARGET_FOUND == true, "No target specified");
