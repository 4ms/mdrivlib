#pragma once
#include "drivers/pin.hh"
#include <concepts>

namespace mdrivlib
{

struct DefaultPinChangeConf {
	static constexpr uint32_t pin = 0;
	static constexpr GPIO port = GPIO::A;
	static constexpr bool on_rising_edge = false;
	static constexpr bool on_falling_edge = false;
	static constexpr uint32_t priority1 = 3;
	static constexpr uint32_t priority2 = 3;
	static constexpr uint32_t core = 1;
};

template<typename T>
concept PinChangeConf = requires(T) {
	requires std::derived_from<T, DefaultPinChangeConf>;
};

} // namespace mdrivlib
