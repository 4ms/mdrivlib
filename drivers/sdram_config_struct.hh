#pragma once
#include "pin.hh"
#include <array>
#include <cstdint>

namespace mdrivlib
{

struct SDRAMTimingConfig {
	uint8_t tRC_ns;	 // Row Cycle time (same bank)
	uint8_t tRP_ns;	 // Precharge to refresh/row activate
	uint8_t tMRD_ns; // Mode register set cycle time? == LoadToActiveDelay
	uint8_t tXSR_ns; // Exit Self Refresh time
	uint8_t tRAS_ns; // Row activate to precharge time == Self-refresh?
	uint8_t tWR_ns;	 // Write Recovery time
	uint8_t tRCD_ns; // RAS to CAS delay (same bank)

	uint8_t max_freq_MHz;
	uint8_t CAS_latency;
	uint8_t refresh_ms;
	bool read_burst_enabled;
	uint8_t read_pipe_delay;
};

struct SDRAMArchConfig {
	uint8_t num_internal_banks;
	uint8_t bus_width_bits;
	uint8_t column_bits;
	uint8_t row_bits;
	uint32_t num_rows;
};

struct SDRAMPinNames {
	PinDef A0;
	PinDef A1;
	PinDef A2;
	PinDef A3;
	PinDef A4;
	PinDef A5;
	PinDef A6;
	PinDef A7;
	PinDef A8;
	PinDef A9;
	PinDef A10;
	PinDef A11;
	PinDef A12;
	PinDef BA0;
	PinDef BA1;
	PinDef D0;
	PinDef D1;
	PinDef D2;
	PinDef D3;
	PinDef D4;
	PinDef D5;
	PinDef D6;
	PinDef D7;
	PinDef D8;
	PinDef D9;
	PinDef D10;
	PinDef D11;
	PinDef D12;
	PinDef D13;
	PinDef D14;
	PinDef D15;
	PinDef NBL0;
	PinDef NBL1;
	PinDef SDCKE0;
	PinDef SDCLK;
	PinDef SDNCAS;
	PinDef SDNE0;
	PinDef SDNRAS;
	PinDef SDNWE;
};
union SDRAMPinList {
	SDRAMPinNames pin_names;
	std::array<PinDef, 39> pin_array;
};

struct SDRAMConfig {
	SDRAMPinList pin_list;
	SDRAMTimingConfig timing;
	SDRAMArchConfig arch;
	uint8_t connected_bank;
	uint32_t fmc_kernel_clock_MHz;
};
} // namespace mdrivlib
