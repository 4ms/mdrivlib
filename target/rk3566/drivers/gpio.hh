#pragma once
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include <cstdint>

namespace mdrivlib
{

struct Gpio {

	// valid for _L registers only
	static uint32_t A(uint32_t x) {
		return x;
	}
	static uint32_t B(uint32_t x) {
		return x + 8;
	}

	// valid for _H registers only
	static uint32_t C(uint32_t x) {
		return x;
	}
	static uint32_t D(uint32_t x) {
		return x + 8;
	}

	enum class Port { A, B, C, D };

	void high(Port port, uint8_t pin) volatile {
		using namespace mdrivlib;
		if (port == Port::A)
			data_L = masked_set_bit(A(pin));

		if (port == Port::B)
			data_L = masked_set_bit(B(pin));

		if (port == Port::C)
			data_H = masked_set_bit(C(pin));

		if (port == Port::D)
			data_H = masked_set_bit(D(pin));
	}

	void low(Port port, uint8_t pin) volatile {
		using namespace mdrivlib;
		if (port == Port::A)
			data_L = masked_clr_bit(A(pin));

		if (port == Port::B)
			data_L = masked_clr_bit(B(pin));

		if (port == Port::C)
			data_H = masked_clr_bit(C(pin));

		if (port == Port::D)
			data_H = masked_clr_bit(D(pin));
	}

	void dir_output(Port port, uint8_t pin) volatile {
		using namespace mdrivlib;
		if (port == Port::A)
			dir_L = masked_set_bit(A(pin));

		if (port == Port::B)
			dir_L = masked_set_bit(B(pin));

		if (port == Port::C)
			dir_H = masked_set_bit(C(pin));

		if (port == Port::D)
			dir_H = masked_set_bit(D(pin));
	}

	void dir_input(Port port, uint8_t pin) volatile {
		using namespace mdrivlib;
		if (port == Port::A)
			dir_L = masked_clr_bit(A(pin));

		if (port == Port::B)
			dir_L = masked_clr_bit(B(pin));

		if (port == Port::C)
			dir_H = masked_clr_bit(C(pin));

		if (port == Port::D)
			dir_H = masked_clr_bit(D(pin));
	}

	// 0 = Output low, 1 = Output high
	uint32_t data_L; // 0x00
	uint32_t data_H; // 0x04

	// 0 = Input, 1 = Output
	enum class PinMode : uint32_t {
		Input = 0,
		Output = 1,
		Alt = 2, // flag to use the gpiomux
	};
	uint32_t dir_L; // 0x08
	uint32_t dir_H; // 0x0C

	uint32_t intr_en_L; // 0x10
	uint32_t intr_en_H;
	uint32_t intr_mask_L;
	uint32_t intr_mask_H;

	// Level 0, Edge 1
	uint32_t intr_type_L; // 0x20
	uint32_t intr_type_H;

	// active low = 0, active high = 1
	enum class PinPolarity {
		Normal = 0,
		Inverted = 1,
	};
	uint32_t intr_pol_L;
	uint32_t intr_pol_H;

	uint32_t intr_bothedge_L; // 0x30
	uint32_t intr_bothedge_H;

	uint32_t debounce_L;
	uint32_t debounce_H;

	uint32_t divide_enable_L; // 0x40
	uint32_t divide_enable_H;

	uint32_t divide_control;
	uint32_t _pad1;

	uint32_t intr_status; // 0x50
	uint32_t _pad2;
	uint32_t intr_rawstatus;
	uint32_t _pad3;

	uint32_t intr_eoi_L; // 0x60
	uint32_t intr_eoi_H;

	uint32_t _pad4;
	uint32_t _pad5;

	uint32_t ext_port; // 0x70

	uint32_t _pad6;

	uint32_t ver_id; // 0x78
};

static inline volatile Gpio *const GPIO0 = reinterpret_cast<Gpio *const>(GPIO0_BASE);
static inline volatile Gpio *const GPIO1 = reinterpret_cast<Gpio *const>(GPIO1_BASE);
static inline volatile Gpio *const GPIO2 = reinterpret_cast<Gpio *const>(GPIO2_BASE);
static inline volatile Gpio *const GPIO3 = reinterpret_cast<Gpio *const>(GPIO3_BASE);
static inline volatile Gpio *const GPIO4 = reinterpret_cast<Gpio *const>(GPIO4_BASE);

enum class GPIO : uint32_t {
	GPIO0 = GPIO0_BASE,
	GPIO1 = GPIO1_BASE,
	GPIO2 = GPIO2_BASE,
	GPIO3 = GPIO3_BASE,
	GPIO4 = GPIO4_BASE,
	Unused = 0,
};

using PinMode = Gpio::PinMode;
using PinPolarity = Gpio::PinPolarity;

// TODO: gpio config registers
enum class PinPull : uint32_t {
	Up,
	Down,
	None,
};

enum class PinSpeed : uint32_t {
	Low,
	Medium,
	High,
	VeryHigh,
};

enum class PinOType : uint32_t {
	PushPull,
	OpenDrain,
};

enum PinNum : uint8_t {
	A0,
	A1,
	A2,
	A3,
	A4,
	A5,
	A6,
	A7,
	B0,
	B1,
	B2,
	B3,
	B4,
	B5,
	B6,
	B7,
	C0,
	C1,
	C2,
	C3,
	C4,
	C5,
	C6,
	C7,
	D0,
	D1,
	D2,
	D3,
	D4,
	D5,
	D6,
	D7,
};

using PinAF = uint8_t;

}; // namespace mdrivlib
