// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
#include "drivers/risab.hh"
#include "drivers/rcc_common.hh"

namespace mdrivlib
{

static void full_access_risab(RISAB_TypeDef *risab) {
	risab->CR = risab->CR | RISAB_CR_SRWIAD;

	for (auto i = 0; i < 32; i++) {
		// Clear SEC flag for all CIDs
		risab->PGSECCFGR[i] = 0UL;
		// Clear PRIV flag for all CIDs
		risab->PGPRIVCFGR[i] = 0UL;
		// Clear CFEN, DCEN, DCID
		risab->PGCIDCFGR[i] = 0UL;
	}
	for (auto cid = 0; cid < 7; cid++) {
		risab->CID[cid].PRIVCFGR = 0;
		risab->CID[cid].RDCFGR = 0;
		risab->CID[cid].WRCFGR = 0;
	}
}

void sysram_enable() {
	RCC_Enable::SYSRAM_::set();
	full_access_risab(RISAB1);
	full_access_risab(RISAB2);
}

void sram1_enable() {
	RCC_Enable::SRAM1_::set();
	full_access_risab(RISAB3);
}

void sram3_enable() {
	RCC_Enable::SRAM2_::set();
	full_access_risab(RISAB4);
}

void retram_enable() {
	RCC_Enable::RETRAM_::set();
	full_access_risab(RISAB5);
}

void vderam_enable() {
	RCC_Enable::VDERAM_::set();
	full_access_risab(RISAB6);
}

} // namespace mdrivlib

extern "C" void block_ram_enable_el3() {
	mdrivlib::sysram_enable();
	mdrivlib::sram1_enable();
	mdrivlib::sram3_enable();
	mdrivlib::retram_enable();
	mdrivlib::vderam_enable();
}
