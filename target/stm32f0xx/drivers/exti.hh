#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib::EXTI_
{
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
constexpr uint8_t PortF = 5;

// Rising/Falling edge trigger selection:
using RTSR = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, RTSR), 0x003FFFFF>;
using FTSR = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, FTSR), 0x003FFFFF>;

// Interrupt and Event enables:
using IMR = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, IMR), 0xFFFFFFFF>;
using EMR = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, EMR), 0xFFFFFFFF>;

// Rising/Falling trigger pending (read, then write 1 to clear)
using PR = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, PR), 0x003FFFFF>;

// Software Interrupt
using SWIER = RegisterBits<ReadWrite, EXTI_BASE + offsetof(EXTI_TypeDef, SWIER), 0x007BFFFF>;

// Rising edge and falling edge trigger selection for GPIO pins:
template<uint32_t PinNum>
using PinRisingTrig = RegisterBits<ReadWrite, RTSR::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinFallingTrig = RegisterBits<ReadWrite, FTSR::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinTrigPending = RegisterBits<ReadWrite, PR::BaseAddress, (1 << PinNum)>;

template<uint32_t PinNum>
using PinInterruptMask = RegisterBits<ReadWrite, IMR::BaseAddress, (1 << PinNum)>;

} // namespace mdrivlib::EXTI_
