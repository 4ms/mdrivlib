#pragma once
#include "drivers/pin.hh"
#include <concepts>

namespace mdrivlib
{

struct DefaultSDCardConf {
	static constexpr uint32_t SDPeriphNum = 0;
	static constexpr uint32_t speed_hz = 0;
	enum Width { Single, Wide4 };
	static constexpr Width width = Wide4;
	static constexpr PinDef D0{};
	static constexpr PinDef D1{};
	static constexpr PinDef D2{};
	static constexpr PinDef D3{};
	static constexpr PinDef SCLK{};
	static constexpr PinDef CMD{};
};

template<typename T>
concept SDCardConfC = requires(T) { requires std::derived_from<T, DefaultSDCardConf>; };

} // namespace mdrivlib
