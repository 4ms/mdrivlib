#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib {

namespace stm32h7x5 {

using SYSCFG_FMP = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, PMCR), 0, 7>;

namespace SYSCFG_EXTI {
using Pin0 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 0, 3>;
using Pin1 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 4, 3>;
using Pin2 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 8, 3>;
using Pin3 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[0]), 12, 3>;
using Pin4 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 0, 3>;
using Pin5 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 4, 3>;
using Pin6 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 8, 3>;
using Pin7 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[1]), 12, 3>;
using Pin8 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 0, 3>;
using Pin9 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 4, 3>;
using Pin10 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 8, 3>;
using Pin11 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[2]), 12, 3>;
using Pin12 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 0, 3>;
using Pin13 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 4, 3>;
using Pin14 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 8, 3>;
using Pin15 = RegisterSection<ReadWrite, SYSCFG_BASE + offsetof(SYSCFG_TypeDef, EXTICR[3]), 12, 3>;

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
} // namespace SYSCFG_EXTI

using EXTI_IMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, IMR1), 0xFFFFFFFF>;
using EXTI_EMR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EMR1), 0xFFFFFFFF>;
using EXTI_RTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR1), 0xFFFFFFFF>;
using EXTI_FTSR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR1), 0xFFFFFFFF>;

using EXTI_PR1 = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, PR1), 0xFFFFFFFF>;

} // namespace stm32h7x5
} // namespace mdrivlib
