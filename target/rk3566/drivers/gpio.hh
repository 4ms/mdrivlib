#include "masks.hh"
#include <stdint.h> // clangd complains about cstdint????

namespace RockchipPeriph
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

} // namespace RockchipPeriph

namespace HW
{

static inline volatile RockchipPeriph::Gpio *const GPIO0 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfdd60000);
static inline volatile RockchipPeriph::Gpio *const GPIO1 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe740000);
static inline volatile RockchipPeriph::Gpio *const GPIO2 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe750000);
static inline volatile RockchipPeriph::Gpio *const GPIO3 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe760000);
static inline volatile RockchipPeriph::Gpio *const GPIO4 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe770000);

}; // namespace HW
