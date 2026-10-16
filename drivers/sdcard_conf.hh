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

	// How card presence is detected:
	// DAT3: DAT3 is read as a GPIO while no card is mounted (needs a pull-down on DAT3;
	//       the card's internal pull-up then pulls it high)
	// None: no detect signal: a card is present if it responds to initialization.
	//       While no card is present, initialization is retried at most every redetect_interval_ms.
	enum class CardDetect { DAT3, None };
	static constexpr CardDetect card_detect = CardDetect::DAT3;
	static constexpr uint32_t redetect_interval_ms = 500;
};

template<typename T>
concept SDCardConfC = requires(T) { requires std::derived_from<T, DefaultSDCardConf>; };

} // namespace mdrivlib
