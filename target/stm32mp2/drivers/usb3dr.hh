#pragma once
#include "drivers/rcc_xbar.hh"
#include "drivers/stm32xx.h"
#include <cstdint>

// USB3DR: the STM32MP25's dual-role USB controller (Synopsys DWC3), run in USB2-only mode
// (high/full speed through USB2PHY2; the COMBOPHY isn't used).
//
// init() does the controller's clock, reset, and PHY sequence (RM0457 83.3.2), the same as the
// verified stm32mp2-baremetal usb-drd example. After it, a device (HAL PCD, DWC3 flavor) or host
// driver can program the core. To switch roles, call init() again with the other role: it resets
// the controller and the PHY.
//
// The controller is a DMA master: its event buffers, TRBs, and data buffers are in memory, and
// the RISAF must let it through (see init_rif()).

namespace mdrivlib
{

struct Usb3DR {
	enum class Role { Device, Host };

	// RIFSC RISUP peripheral id (stm32mp25-rif.h STM32MP25_RIFSC_USB3DR_ID) and RIMU master index
	// (TF-A STM32MP2_RIMU_USB3DR)
	static constexpr uint32_t RifscId = 66;
	static constexpr uint32_t RimuId = 4;

	// Call from a secure context that can write the RIFSC (the A35 at EL3), before the core that
	// runs USB uses it. Makes the registers secure and privileged, and makes the controller's DMA
	// secure and privileged with compartment ID master_cid, so it gets through the same RISAF
	// regions as that core. (Same as TF-A's USB programmer setup, which uses CID1.)
	static void init_rif(uint32_t master_cid) {
		RISC->SECCFGR[RifscId / 32] = RISC->SECCFGR[RifscId / 32] | (1u << (RifscId % 32));
		RISC->PRIVCFGR[RifscId / 32] = RISC->PRIVCFGR[RifscId / 32] | (1u << (RifscId % 32));

		RIMC->ATTR[RimuId] = RIMC_ATTR_MPRIV | RIMC_ATTR_MSEC | RIMC_ATTR_CIDSEL |
							 ((master_cid << RIMC_ATTR_MCID_Pos) & RIMC_ATTR_MCID_Msk);
	}

	static void init(Role role) {
		RCC->USB2PHY2CFGR = RCC->USB2PHY2CFGR | RCC_USB2PHY2CFGR_USB2PHY2EN;
		RCC->USB3DRDCFGR = RCC->USB3DRDCFGR | RCC_USB3DRDCFGR_USB3DRDEN;
		RCC->USB3PCIEPHYCFGR = RCC->USB3PCIEPHYCFGR | RCC_USB3PCIEPHYCFGR_USB3PCIEPHYEN;

		// Keep the PHY's PLL running in Suspend: when USB2PHY2CMN=1 the PHY stops the UTMI clock
		// to the controller in Suspend, and the controller stops generating endpoint events
		SYSCFG->USB2PHY2CR = SYSCFG->USB2PHY2CR & ~SYSCFG_USB2PHY2CR_USB2PHY2CMN;

		// 1. Hold the PHYs and the controller in reset
		RCC->USB2PHY2CFGR = RCC->USB2PHY2CFGR | RCC_USB2PHY2CFGR_USB2PHY2RST;
		RCC->USB3DRDCFGR = RCC->USB3DRDCFGR | RCC_USB3DRDCFGR_USB3DRDRST;
		RCC->USB3PCIEPHYCFGR = RCC->USB3PCIEPHYCFGR | RCC_USB3PCIEPHYCFGR_USB3PCIEPHYRST;

		// 2. USB2PHY2 reference clock: flexgen ch58 = HSE (40MHz) / 2 = 20MHz
		FlexbarConf{.PLL = FlexbarConf::PLLx::HSE, .findiv = 1, .prediv = 0}.init(UsbPhyRefFlexgenChan);
		RCC->USB2PHY2CFGR = RCC->USB2PHY2CFGR & ~RCC_USB2PHY2CFGR_USB2PHY2CKREFSEL; // 0: flexgen ch58
		SYSCFG->USB2PHY2CR = (SYSCFG->USB2PHY2CR & ~SYSCFG_USB2PHY2CR_USB2PHY2SEL_Msk) |
							 (UsbPhyRefSel_20MHz << SYSCFG_USB2PHY2CR_USB2PHY2SEL_Pos);

		// 5. COMBOPHY in PCIe mode (not USB3), and the controller in USB2-only mode
		SYSCFG->COMBOPHYCR2 = SYSCFG->COMBOPHYCR2 & ~SYSCFG_COMBOPHYCR2_COMBOPHY_MODESEL;
		if (role == Role::Host)
			SYSCFG->USB3DRCR = (SYSCFG->USB3DRCR | SYSCFG_USB3DRCR_USB3DR_USB2ONLYH) &
							   ~SYSCFG_USB3DRCR_USB3DR_USB2ONLYD;
		else
			SYSCFG->USB3DRCR = (SYSCFG->USB3DRCR | SYSCFG_USB3DRCR_USB3DR_USB2ONLYD) &
							   ~SYSCFG_USB3DRCR_USB3DR_USB2ONLYH;

		// 8. Wait 10us. 9. Release the PHYs. 10. Wait 260us for the PHY PLL to lock.
		HAL_Delay(1);
		RCC->USB2PHY2CFGR = RCC->USB2PHY2CFGR & ~RCC_USB2PHY2CFGR_USB2PHY2RST;
		RCC->USB3PCIEPHYCFGR = RCC->USB3PCIEPHYCFGR & ~RCC_USB3PCIEPHYCFGR_USB3PCIEPHYRST;
		HAL_Delay(1);

		// 11. Release the controller
		RCC->USB3DRDCFGR = RCC->USB3DRDCFGR & ~RCC_USB3DRDCFGR_USB3DRDRST;

		if (role == Role::Host) {
			// Host: VBUS is always valid (we source it)
			SYSCFG->USB2PHY2CR = (SYSCFG->USB2PHY2CR & ~(SYSCFG_USB2PHY2CR_VBUSVLDEXT |
														 SYSCFG_USB2PHY2CR_VBUSVLDEXTSEL)) |
								 SYSCFG_USB2PHY2CR_VBUSVALID;
		} else {
			// Device: VBUS valid comes from set_vbus_valid(), not the PHY's comparator
			SYSCFG->USB2PHY2CR = (SYSCFG->USB2PHY2CR & ~(SYSCFG_USB2PHY2CR_VBUSVLDEXT |
														 SYSCFG_USB2PHY2CR_VBUSVALID)) |
								 SYSCFG_USB2PHY2CR_VBUSVLDEXTSEL;
		}
	}

	// Device mode: tell the controller whether a host is supplying VBUS (it only connects to the
	// bus while VBUS is valid)
	static void set_vbus_valid(bool valid) {
		if (valid)
			SYSCFG->USB2PHY2CR = SYSCFG->USB2PHY2CR | SYSCFG_USB2PHY2CR_VBUSVLDEXT;
		else
			SYSCFG->USB2PHY2CR = SYSCFG->USB2PHY2CR & ~SYSCFG_USB2PHY2CR_VBUSVLDEXT;
	}

	static void deinit() {
		RCC->USB3DRDCFGR = RCC->USB3DRDCFGR | RCC_USB3DRDCFGR_USB3DRDRST;
		RCC->USB2PHY2CFGR = RCC->USB2PHY2CFGR | RCC_USB2PHY2CFGR_USB2PHY2RST;
		RCC->USB2PHY2CFGR = RCC->USB2PHY2CFGR & ~RCC_USB2PHY2CFGR_USB2PHY2EN;
		RCC->USB3DRDCFGR = RCC->USB3DRDCFGR & ~RCC_USB3DRDCFGR_USB3DRDEN;
	}

private:
	static constexpr unsigned UsbPhyRefFlexgenChan = 58;
	// SYSCFG USB2PHY2CR.USB2PHY2SEL: 0b000 = 19.2MHz, 0b001 = 20MHz, 0b010 = 24MHz
	static constexpr uint32_t UsbPhyRefSel_20MHz = 0b001;
};

} // namespace mdrivlib
