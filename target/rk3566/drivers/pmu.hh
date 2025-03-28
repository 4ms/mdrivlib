#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include <cstdint>

namespace mdrivlib::RockchipPeriph
{

namespace Pmu
{

struct CRU_PWR {
	enum Registers {
		CON = PMU_CRU_PWR_BASE + 0,
		SFTCON = PMU_CRU_PWR_BASE + 4,
		STATE = PMU_CRU_PWR_BASE + 4,
	};
};

using alive_osc_ena = RegisterMasked16BitNum<CRU_PWR::CON, 4>;
using alive_osc_ena_sft = RegisterMasked16BitNum<CRU_PWR::SFTCON, 4>;

} // namespace Pmu

} // namespace mdrivlib::RockchipPeriph
