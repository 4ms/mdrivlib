#pragma once
#include <cstdint>

namespace mdrivlib
{

// TRM Chapter 2 (CRU: Clock & Reset Unit)
inline constexpr uint32_t CRU_BASE = 0xFDD20000;
inline constexpr uint32_t CRU_CLKSEL_BASE = 0xFDD20100;
inline constexpr uint32_t CRU_GATE_BASE = 0xFDD20300;
inline constexpr uint32_t CRU_SOFTRST_BASE = 0xFDD20400;

// TRM Chapter 3 (GRF: General Register Files)
inline constexpr uint32_t PMU_GRF_BASE = 0xFDC20000;
inline constexpr uint32_t PMU_GRF_SOC_BASE = PMU_GRF_BASE + 0x100;

inline constexpr uint32_t CPU_GRF_BASE = 0xFDC30000;

inline constexpr uint32_t SYS_GRF_BASE = 0xFDC60000;

// TRM Chapter 7: (PMU: Power Management Unit)
inline constexpr uint32_t PMU_BASE = 0xFDD90000;
inline constexpr uint32_t PMU_CRU_PWR_BASE = 0xFDD900B0;

// GPIO
inline constexpr uint32_t GPIO0_BASE = 0xfdd60000;
inline constexpr uint32_t GPIO1_BASE = 0xfe740000;
inline constexpr uint32_t GPIO2_BASE = 0xfe750000;
inline constexpr uint32_t GPIO3_BASE = 0xfe760000;
inline constexpr uint32_t GPIO4_BASE = 0xfe770000;

// I2C
inline constexpr uint32_t I2C0_BASE = 0xfdd40000;
inline constexpr uint32_t I2C1_BASE = 0xfe5a0000;
inline constexpr uint32_t I2C2_BASE = 0xfe5b0000;
inline constexpr uint32_t I2C3_BASE = 0xfe5c0000;
inline constexpr uint32_t I2C4_BASE = 0xfe5d0000;
inline constexpr uint32_t I2C5_BASE = 0xfe5e0000;

// I2S
inline constexpr uint32_t I2S0_8CH_BASE = 0xfe400000;
inline constexpr uint32_t I2S1_8CH_BASE = 0xfe410000;
inline constexpr uint32_t I2S2_2CH_BASE = 0xfe420000;
inline constexpr uint32_t I2S3_2CH_BASE = 0xfe430000;

} // namespace mdrivlib
