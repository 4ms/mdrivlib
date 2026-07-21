// Generic-timer (CNTPCT) timebase for the A35: provides HAL_GetTick(), udelay(),
// mdelay(), and the SystemA35_* hooks the ST HAL expects. Call
// SystemA35_SYSTICK_Config() once at startup (before using the ST HAL).
// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
#include "drivers/system_reg.hh"
#include <cstdint>

using namespace mdrivlib;

static uint32_t cntfreq_kHz;
static uint32_t cntfreq_MHz;

extern "C" uint32_t SystemA35_SYSTICK_Config(uint32_t timer_priority)
{
	auto cntfreq_Hz = read_cntfreq();	// ticks per second
	cntfreq_kHz = cntfreq_Hz / 1000;	// ticks per ms
	cntfreq_MHz = cntfreq_Hz / 1000000; // ticks per us

	// print("CNT Freq = ", cntfreq, "\n");
	// cntfreq is 64'000'000
	// cntfreq_kHz is 64'000
	// cntfreq_MHz is 64

	cntp_enable(false);
	cntp_irq_enable(false);

	// Clear stale compare by reading cval
	auto now = read_cntpct();

	cntp_enable(true);
	return 0;
}

extern "C" void udelay(unsigned int us)
{
	auto now = read_cntpct();
	while ((read_cntpct() - now) < (us * cntfreq_MHz))
		;
}

extern "C" void mdelay(unsigned int ms)
{
	auto now = read_cntpct();
	while ((read_cntpct() - now) < (ms * cntfreq_kHz))
		;
}

extern "C" uint32_t HAL_GetTick(void)
{
	return read_cntpct() / cntfreq_kHz;
}

extern "C" uint32_t SystemA35_TZ_STGEN_Start(uint32_t ck_ker_clk_freq)
{
	return 0;
}

extern "C" uint32_t SystemA35_SYSTICK_TimerSourceConfig(uint32_t timer_source)
{
	return 0;
}

extern "C" uint32_t SystemA35_ManageTick(uint32_t suspend_resume_stop_tick)
{
	return 0;
}
