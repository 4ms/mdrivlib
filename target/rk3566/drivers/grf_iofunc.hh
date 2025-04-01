#pragma once
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"

namespace mdrivlib
{

namespace GrfIofunc
{

enum Registers {
	SEL0 = 0x0300,
	SEL1 = 0x0304,
	SEL2 = 0x0308,
	SEL3 = 0x030C,
	SEL4 = 0x0310,
};

enum class choice_iomux2 {
	m0 = 0b00,
	m1 = 0b01,
};
enum class choice_iomux3 {
	m0 = 0b00,
	m1 = 0b01,
	m2 = 0b10,
};

using i2c2_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL0, 0b1, 14, choice_iomux2>;
using i2c3_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL1, 0b1, 0, choice_iomux2>;
using i2c4_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL1, 0b1, 2, choice_iomux2>;
using i2c5_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL1, 0b1, 4, choice_iomux2>;
using pwm8_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL1, 0b1, 14, choice_iomux2>;
using pwm11_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL2, 0b11, 4, choice_iomux2>;
using i2s1_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL4, 0b11, 10, choice_iomux3>;
using i2s2_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL4, 0b1, 12, choice_iomux2>;
using i2s3_iomux_sel = RegisterMaskedChoice<SYS_GRF_BASE + SEL4, 0b1, 14, choice_iomux2>;

} // namespace GrfIofunc

} // namespace mdrivlib
