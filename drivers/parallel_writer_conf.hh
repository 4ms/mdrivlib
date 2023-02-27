#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct ParallelWriterConf {
	static constexpr size_t BusWidth = 0;
	static constexpr PinDef data[BusWidth]{};
	static constexpr PinDef chip_sel{};
	static constexpr PinDef write_latch{};
	static constexpr PinDef datacmd_sel{};
	static constexpr PinDef reset{};

	static constexpr uint32_t DataSetupTime = 0;
	static constexpr uint32_t WriteLatchAfterDelay = 0;
};

} // namespace mdrivlib
