// Generic-timer (CNTPCT) timebase for the A35: provides HAL_GetTick(), udelay(),
// mdelay(), and the SystemA35_* hooks the ST HAL expects.
// The tick works from reset: the counter frequency is read on first use, so
// HAL_Delay() etc. are safe in C++ static constructors, before HAL_Init().
// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
#include "drivers/system_reg.hh"
#include <cstdint>

using namespace mdrivlib;

static uint32_t cntfreq_kHz;
static uint32_t cntfreq_MHz;

static void read_counter_freq()
{
	auto cntfreq_Hz = read_cntfreq();	// ticks per second (CNTFRQ_EL0, set by TF-A)
	cntfreq_kHz = cntfreq_Hz / 1000;	// ticks per ms
	cntfreq_MHz = cntfreq_Hz / 1000000; // ticks per us
}

static uint32_t ticks_per_ms()
{
	if (cntfreq_kHz == 0) [[unlikely]]
		read_counter_freq();
	return cntfreq_kHz;
}

static uint32_t ticks_per_us()
{
	if (cntfreq_MHz == 0) [[unlikely]]
		read_counter_freq();
	return cntfreq_MHz;
}

extern "C" uint32_t SystemA35_SYSTICK_Config(uint32_t timer_priority)
{
	read_counter_freq();

	// print("CNT Freq = ", cntfreq, "\n");
	// cntfreq is 64'000'000
	// cntfreq_kHz is 64'000
	// cntfreq_MHz is 64

	cntp_enable(false);
	cntp_irq_enable(false);

	cntp_enable(true);
	return 0;
}

extern "C" void udelay(unsigned int us)
{
	auto now = read_cntpct();
	while ((read_cntpct() - now) < (us * ticks_per_us()))
		;
}

extern "C" void mdelay(unsigned int ms)
{
	auto now = read_cntpct();
	while ((read_cntpct() - now) < (ms * ticks_per_ms()))
		;
}

extern "C" uint32_t HAL_GetTick(void)
{
	return read_cntpct() / ticks_per_ms();
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
