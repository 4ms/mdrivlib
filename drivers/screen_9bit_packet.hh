#pragma once
#include <cstdint>

namespace mdrivlib
{

struct Screen9BitPacket {
	enum class Type { Data, Cmd, DelayMS } action;
	uint8_t payload;
};

} // namespace mdrivlib
