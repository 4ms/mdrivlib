#pragma once
#include <array>
#include <cstdint>

namespace mdrivlib
{

enum RotaryStepSize { RotaryHalfStep = 0, RotaryFullStep = 6 };

namespace RotaryEncDetails
{

enum HalfStepRotaryState : uint8_t {
	HS_R_START = 0x0,
	HS_R_CCW_BEGIN = 0x01,
	HS_R_CW_BEGIN = 0x02,
	HS_R_START_M = 0x03,
	HS_R_CW_BEGIN_M = 0x04,
	HS_R_CCW_BEGIN_M = 0x05,
	HS_DIR_CW = 0x10,
	HS_DIR_CCW = 0x20,
};

enum FullStepRotaryState : uint8_t {
	FS_R_START = 0x0,
	FS_R_CW_FINAL = 0x01,
	FS_R_CW_BEGIN = 0x02,
	FS_R_CW_NEXT = 0x03,
	FS_R_CCW_BEGIN = 0x04,
	FS_R_CCW_FINAL = 0x05,
	FS_R_CCW_NEXT = 0x06,
	FS_DIR_CW = 0x10,
	FS_DIR_CCW = 0x20,
};

using StateTableType = std::array<std::array<uint8_t, 4>, 7>;

template<RotaryStepSize StepSize>
StateTableType RotaryStateTable;

template<>
inline constexpr StateTableType RotaryStateTable<RotaryHalfStep> = {{
	// HalfStep
	// R_START (00)
	{HS_R_START_M, HS_R_CW_BEGIN, HS_R_CCW_BEGIN, HS_R_START},

	// R_CCW_BEGIN
	{HS_R_START_M | HS_DIR_CCW, HS_R_START, HS_R_CCW_BEGIN, HS_R_START},

	// HS_R_CW_BEGIN
	{HS_R_START_M | HS_DIR_CW, HS_R_CW_BEGIN, HS_R_START, HS_R_START},

	// HS_R_START_M (11)
	{HS_R_START_M, HS_R_CCW_BEGIN_M, HS_R_CW_BEGIN_M, HS_R_START},

	// R_CW_BEGIN_M
	{HS_R_START_M, HS_R_START_M, HS_R_CW_BEGIN_M, HS_R_START | HS_DIR_CW},

	// R_CCW_BEGIN_M
	{HS_R_START_M, HS_R_CCW_BEGIN_M, HS_R_START_M, HS_R_START | HS_DIR_CCW},
	{},

}};

template<>
inline constexpr StateTableType RotaryStateTable<RotaryFullStep> = {{
	// FullStep
	// FS_R_START
	{FS_R_START, FS_R_CW_BEGIN, FS_R_CCW_BEGIN, FS_R_START},

	// R_CW_FINAL
	{FS_R_CW_NEXT, FS_R_START, FS_R_CW_FINAL, FS_R_START | FS_DIR_CW},

	// R_CW_BEGIN
	{FS_R_CW_NEXT, FS_R_CW_BEGIN, FS_R_START, FS_R_START},

	// R_CW_NEXT
	{FS_R_CW_NEXT, FS_R_CW_BEGIN, FS_R_CW_FINAL, FS_R_START | FS_DIR_CW},

	// R_CCW_BEGIN
	{FS_R_CCW_NEXT, FS_R_START, FS_R_CCW_BEGIN, FS_R_START},

	// R_CCW_FINAL
	{FS_R_CCW_NEXT, FS_R_CCW_FINAL, FS_R_START, FS_R_START | FS_DIR_CCW},

	// R_CCW_NEXT
	{FS_R_CCW_NEXT, FS_R_CCW_FINAL, FS_R_CCW_BEGIN, FS_R_START | FS_DIR_CCW},
}};

} // namespace RotaryEncDetails

} // namespace mdrivlib
