#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct RamTest {
	static uint32_t test(const uint32_t ram_start, const uint32_t ram_size) {

		uint32_t num_fails = 0;

		auto countup_func = [](uint32_t x) { return x; };
		num_fails += do_sdram_test(countup_func, ram_start, ram_size);

		auto bitinvert_countdown_func = [](uint32_t x) { return 0xFFFFFFFFU - x; };
		num_fails += do_sdram_test(bitinvert_countdown_func, ram_start, ram_size);

		constexpr uint32_t freq = 987654321;
		auto thrash_func = [](uint32_t x) { return x * freq; };
		num_fails += do_sdram_test(thrash_func, ram_start, ram_size);

		return num_fails;
	}

	static uint32_t do_sdram_test(uint32_t (*mapfunc)(uint32_t), const uint32_t ram_start, const uint32_t ram_size) {
		uint32_t num_fails = 0;
		const size_t test_val_size = sizeof(uint32_t);

		uint32_t addr = ram_start;
		for (uint32_t i = 0; i < (ram_size / test_val_size); i++) {
			// wait_until_ready();
			uint32_t testval = mapfunc(i);
			*((uint32_t *)addr) = testval;

			addr += test_val_size;
		}

		addr = ram_start;
		for (uint32_t i = 0; i < (ram_size / test_val_size); i++) {
			uint32_t readval = *((uint32_t *)addr);

			uint32_t expectedval = mapfunc(i);
			if (readval != expectedval)
				num_fails++;

			addr += test_val_size;
		}
		return num_fails;
	}
};
} // namespace mdrivlib
