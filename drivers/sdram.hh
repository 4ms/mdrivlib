#pragma once
#include "sdram_config_struct.hh"
#include "stm32xx.h"

namespace SDRAMPeriphMath
{

inline uint32_t freq_to_clockdiv(uint32_t FMC_clock, uint32_t freq) {
	// Requirement: FMC_clock / clockdiv >= freq
	// STM32H7 SDRAM controller: Only values of 2 and 3 are valid for clockdiv
	return ((freq * 2 * 1000000) >= FMC_clock) ? 2 : 3;
};

constexpr uint32_t ns_to_hclks(const uint32_t sdram_clock, const uint32_t ns) {
	// return 16;
	uint32_t sdram_mhz = sdram_clock / 1000000U;
	uint32_t clks_x1000 = ns * sdram_mhz;
	uint32_t clks_x10 = clks_x1000 / 100;
	uint32_t round_up_clks_x10 = clks_x10 + 9;
	return round_up_clks_x10 / 10;
};

} // namespace SDRAMPeriphMath

namespace mdrivlib
{

class SDRAMPeriph {
public:
	SDRAMPeriph(const SDRAMConfig &sdram_defs, SDRAMBank bank, uint32_t fmc_kernel_clock_MHz);
	static bool is_busy();
	static void wait_until_ready();

	uint32_t status;

private:
	const SDRAMConfig &defs;
	void config_timing(SDRAMBank bank, uint32_t fmc_kernel_clock_MHz);
	void start_refresh(SDRAMBank bank);
	bool time_test(uint32_t start_addr, uint32_t size_bytes);
	void init_gpio();

	uint32_t sdram_clock_;
};

} // namespace mdrivlib
