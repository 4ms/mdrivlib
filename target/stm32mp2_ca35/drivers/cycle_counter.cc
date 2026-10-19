#include "drivers/cycle_counter.hh"
#include "drivers/system_reg.hh"

namespace mdrivlib
{
void CycleCounter::init() {
}

uint32_t CycleCounter::read_cycle_count() {
	return read_cntpct(); // The generic timer (64MHz), so not really a cycle counter, but accurate enough.
}
} // namespace mdrivlib
