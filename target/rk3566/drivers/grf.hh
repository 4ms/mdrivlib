#pragma once

#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include "gpiomux.hh"

namespace Rockchip
{

struct PMU {
	GPIO0A_IOMUX_L gpio0_a_l;
	GPIO0A_IOMUX_H gpio0_a_h;
	GPIO0B_IOMUX_L gpio0_b_l;
	GPIO0B_IOMUX_H gpio0_b_h;
	GPIO0C_IOMUX_L gpio0_c_l;
	GPIO0C_IOMUX_H gpio0_c_h;
	GPIO0D_IOMUX_L gpio0_d_l;
	GPIO0D_IOMUX_H gpio0_d_h;
	// TODO: finish the registers in the pmu
};

struct SYS {
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

	// TODO: finish the registers in the sys
};

} // namespace Rockchip

namespace mdrivlib::RockchipPeriph
{
namespace GRF_SOC
{

enum class con1_i2s1_mclk_sel {
	i2s1_mclk_rx = 0,
	i2s1_mclk_tx = 1,
};
using i2s1_mclk_sel = RegisterMaskedChoice<SYS_GRF_BASE + 0x0504, 1 << 5, con1_i2s1_mclk_sel>;

enum class con2_i2s1_mclk_oe {
	from_ext_chip = 0,
	from_cru = 1,
};
using i2s1_mclk_tx_oe = RegisterMaskedChoice<SYS_GRF_BASE + 0x0508, 1 << 1, con2_i2s1_mclk_oe>;
using i2s1_mclk_rx_oe = RegisterMaskedChoice<SYS_GRF_BASE + 0x0508, 1 << 0, con2_i2s1_mclk_oe>;

} // namespace GRF_SOC
} // namespace mdrivlib::RockchipPeriph

namespace HW
{

static inline volatile Rockchip::PMU *const PMU = reinterpret_cast<Rockchip::PMU *>(0xfdc20000);
static inline volatile Rockchip::SYS *const SYS =
	reinterpret_cast<Rockchip::SYS *>(mdrivlib::RockchipPeriph::SYS_GRF_BASE);

}; // namespace HW
