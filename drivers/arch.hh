#pragma once

#if defined(STM32H745xx) || defined(STM32H755xx)
namespace mdrivlib
{
namespace stm32h7x5
{
//
} // namespace stm32h7x5
} // namespace mdrivlib

namespace target = mdrivlib::stm32h7x5;
#endif

#if defined(STM32F746xx)

namespace mdrivlib
{
namespace stm32f7xx
{
//
} // namespace stm32f7xx
} // namespace mdrivlib

namespace target = mdrivlib::stm32f7xx;
#endif

