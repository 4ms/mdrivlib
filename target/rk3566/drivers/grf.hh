#pragma once
#include "drivers/gpiomux.hh"
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"

namespace mdrivlib
{

namespace RockchipPeriph
{

namespace PmuGrf
{

struct GPIO_IOMUX {
	GPIO0A_IOMUX_L gpio0_a_l;
	GPIO0A_IOMUX_H gpio0_a_h;
	GPIO0B_IOMUX_L gpio0_b_l;
	GPIO0B_IOMUX_H gpio0_b_h;
	GPIO0C_IOMUX_L gpio0_c_l;
	GPIO0C_IOMUX_H gpio0_c_h;
	GPIO0D_IOMUX_L gpio0_d_l;
	GPIO0D_IOMUX_H gpio0_d_h;
};

struct SOC {
	enum Registers {
		CON0 = PMU_GRF_SOC_BASE + 0x0,
		CON1 = PMU_GRF_SOC_BASE + 0x4,
		CON2 = PMU_GRF_SOC_BASE + 0x8,
		CON3 = PMU_GRF_SOC_BASE + 0xC,
		CON4 = PMU_GRF_SOC_BASE + 0x10,
	};
};

enum class choice_pwm2_iomux {
	m0 = 0b00,
	m1 = 0b01,
};

using pwm2_iomux_sel = RegisterMaskedChoice<SOC::CON4, 0b11, 4, choice_pwm2_iomux>;

} // namespace PmuGrf

namespace SysGrf
{

struct GPIO_IOMUX {
	GPIO1A_IOMUX_L gpio1_a_l;
	GPIO1A_IOMUX_H gpio1_a_h;
	GPIO1B_IOMUX_L gpio1_b_l;
	GPIO1B_IOMUX_H gpio1_b_h;
	GPIO1C_IOMUX_L gpio1_c_l;
	GPIO1C_IOMUX_H gpio1_c_h;
	GPIO1D_IOMUX_L gpio1_d_l;
	GPIO1D_IOMUX_H gpio1_d_h;
	GPIO2A_IOMUX_L gpio2_a_l;
	GPIO2A_IOMUX_H gpio2_a_h;
	GPIO2B_IOMUX_L gpio2_b_l;
	GPIO2B_IOMUX_H gpio2_b_h;
	GPIO2C_IOMUX_L gpio2_c_l;
	GPIO2C_IOMUX_H gpio2_c_h;
	GPIO2D_IOMUX_L gpio2_d_l;
	GPIO2D_IOMUX_H gpio2_d_h;
	GPIO3A_IOMUX_L gpio3_a_l;
	GPIO3A_IOMUX_H gpio3_a_h;
	GPIO3B_IOMUX_L gpio3_b_l;
	GPIO3B_IOMUX_H gpio3_b_h;
	GPIO3C_IOMUX_L gpio3_c_l;
	GPIO3C_IOMUX_H gpio3_c_h;
	GPIO3D_IOMUX_L gpio3_d_l;
	GPIO3D_IOMUX_H gpio3_d_h;
	GPIO4A_IOMUX_L gpio4_a_l;
	GPIO4A_IOMUX_H gpio4_a_h;
	GPIO4B_IOMUX_L gpio4_b_l;
	GPIO4B_IOMUX_H gpio4_b_h;
	GPIO4C_IOMUX_L gpio4_c_l;
	GPIO4C_IOMUX_H gpio4_c_h;
	GPIO4D_IOMUX_L gpio4_d_l;
};

enum class con1_i2s1_mclk_sel {
	i2s1_mclk_rx = 0,
	i2s1_mclk_tx = 1,
};
using i2s1_mclk_sel = RegisterMaskedChoice<SYS_GRF_BASE + 0x0504, 1, 5, con1_i2s1_mclk_sel>;

enum class con2_i2s1_mclk_oe {
	from_ext_chip = 0,
	from_cru = 1,
};
using i2s1_mclk_tx_oe = RegisterMaskedChoice<SYS_GRF_BASE + 0x0508, 0b1, 1, con2_i2s1_mclk_oe>;
using i2s1_mclk_rx_oe = RegisterMaskedChoice<SYS_GRF_BASE + 0x0508, 0b1, 0, con2_i2s1_mclk_oe>;

} // namespace SysGrf

} // namespace RockchipPeriph

static inline volatile RockchipPeriph::PmuGrf::GPIO_IOMUX *const PMU_GPIO_IOMUX =
	reinterpret_cast<RockchipPeriph::PmuGrf::GPIO_IOMUX *>(PMU_GRF_BASE);

static inline volatile RockchipPeriph::SysGrf::GPIO_IOMUX *const SYS_GPIO_IOMUX =
	reinterpret_cast<RockchipPeriph::SysGrf::GPIO_IOMUX *>(SYS_GRF_BASE);

} // namespace mdrivlib
