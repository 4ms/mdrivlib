#pragma once

namespace mdrivlib
{

static inline void target_sdram_post_init() {
	__FMC_ENABLE();
	// FMC_Bank1_R->BTCR[0] |= FMC_BCR1_FMCEN;
}

static inline void target_sdram_pre_init() {
	// H7x5: pins PC_2C and PC_3C must be configured. See RM0399 Rev 3, page 588
	RCC_Enable::SYSCFG_::set();
	SYSCFG->PMCR = SYSCFG->PMCR & ~(SYSCFG_PMCR_PC2SO_Msk | SYSCFG_PMCR_PC3SO_Msk);
}
} // namespace mdrivlib
