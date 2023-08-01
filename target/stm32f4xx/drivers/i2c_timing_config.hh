#pragma once
#include <cstdint>

namespace mdrivlib
{

struct I2CTimingConfig {
	uint32_t speed_hz;

	[[nodiscard]] constexpr uint32_t calc() const {
		return speed_hz;
	}
};

} // namespace mdrivlib
