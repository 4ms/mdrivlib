#include "gic.hh"

namespace mdrivlib
{

void IRQ_init() {
	// printf("Waking CPU GICR:\n");
	HW::GICRedist->Wake();

	uint64_t pmr = 0xFF;
	// printf("Set PMR (min priority) via GICC to %lu\n", pmr);
	GIC_SetInterfacePriorityMask(pmr);
	// printf("Read PMR back %u\n", GIC_GetInterfacePriorityMask());

	// printf("Set BPR (priority point)\n");
	GIC_SetBinaryPoint(4);		 // Group priority: [7:4], Subpriority [3:0]
	GIC_SetBinaryPointGroup1(2); //not used?

	// printf("Set EOI mode to single step\n");
	GIC_SetEOIModeTwoStep(false);

	// printf("Enable Group1NS in GICD CTRL\n");
	GIC_EnableGroup1NS();

	// printf("Set Routing mode to 1\n");
	GIC_SetRoutingMode(1);
}

} // namespace mdrivlib
