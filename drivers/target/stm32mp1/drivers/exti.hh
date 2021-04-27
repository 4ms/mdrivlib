#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
namespace stm32mp1
{
namespace core_ca7
{

namespace EXTI_
{
using Pin0 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 0, 8>;
using Pin1 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 8, 8>;
using Pin2 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 16, 8>;
using Pin3 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[0]), 24, 8>;
using Pin4 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 0, 8>;
using Pin5 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 8, 8>;
using Pin6 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 16, 8>;
using Pin7 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[1]), 24, 8>;
using Pin8 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 0, 8>;
using Pin9 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 8, 8>;
using Pin10 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 16, 8>;
using Pin11 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[2]), 24, 8>;
using Pin12 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 0, 8>;
using Pin13 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 8, 8>;
using Pin14 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 16, 8>;
using Pin15 = RegisterSection<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EXTICR[3]), 24, 8>;

constexpr uint8_t PortA = 0;
constexpr uint8_t PortB = 1;
constexpr uint8_t PortC = 2;
constexpr uint8_t PortD = 3;
constexpr uint8_t PortE = 4;
constexpr uint8_t PortF = 5;
constexpr uint8_t PortG = 6;
constexpr uint8_t PortH = 7;
constexpr uint8_t PortI = 8;
constexpr uint8_t PortJ = 9;
constexpr uint8_t PortK = 10;
constexpr uint8_t PortZ = 11;
} // namespace EXTI_

using EXTI_C1IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C1IMR1), 0xFFFFFFFF>;
using EXTI_C2IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2IMR1), 0xFFFFFFFF>;
using EXTI_C2EMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, C2EMR1), 0xFFFFFFFF>;
using EXTI_RTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR1), 0xFFFFFFFF>;
using EXTI_FTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR1), 0xFFFFFFFF>;

// using EXTI_PR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, PR1), 0xFFFFFFFF>;

} // namespace core_ca7
} // namespace stm32mp1
} // namespace mdrivlib
