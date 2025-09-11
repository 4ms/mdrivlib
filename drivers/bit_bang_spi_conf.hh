#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct BitBangSpiTxConf {
	static constexpr PinDef data{};
	static constexpr PinDef chip_sel{};
	static constexpr PinDef clock{};

	static constexpr PinPolarity clk_polarity = PinPolarity::Normal;

	static constexpr uint32_t DataSetupTime = 0;
	static constexpr uint32_t ClockLowTime = 0;
	static constexpr uint32_t ClockHighTime = 0;
	static constexpr uint32_t ChipSelectSetupTime = 0;
	static constexpr uint32_t WriteLatchAfterDelay = 0;
};

} // namespace mdrivlib
