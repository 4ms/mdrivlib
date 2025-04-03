#pragma once
#include <cstdint>

struct DMA_Config {
	uint32_t DMA_periph_num;
	uint32_t DMA_stream_num;
	uint32_t channel_request;
	uint32_t IRQn;
	uint32_t pri;
	uint32_t subpri;
	bool continuous;
};
