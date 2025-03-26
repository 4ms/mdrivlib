#pragma once
#include <cstdint>

namespace mdrivlib::RockchipPeriph
{

inline constexpr uint32_t CRU_CLKSEL_BASE = 0xFDD20100;
inline constexpr uint32_t CRU_GATE_BASE = 0xFDD20300;
inline constexpr uint32_t CRU_SOFTRST_BASE = 0xFDD20400;

inline constexpr uint32_t SYS_GRF_BASE = 0xFDC60000;

} // namespace mdrivlib::RockchipPeriph
