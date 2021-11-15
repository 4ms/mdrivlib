#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct ParallelWriterConf {
	static constexpr size_t BusWidth = 0;
	static constexpr PinNoInit data[BusWidth]{};
	static constexpr PinNoInit chip_sel{};
	static constexpr PinNoInit data_out{};
	static constexpr PinNoInit write_latch{};
	static constexpr PinNoInit datacmd_sel{};
};

} // namespace mdrivlib
