#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct ParallelWriterConf {
	static constexpr size_t BusWidth = 0;
	static constexpr PinNoInit data[BusWidth]{};
	static constexpr PinNoInit chip_sel{};
	static constexpr PinNoInit write_latch{};
	static constexpr PinNoInit datacmd_sel{};

	static constexpr uint32_t DataSetupTime = 0;
	static constexpr uint32_t WriteLatchAfterDelay = 0;
};

} // namespace mdrivlib
