#pragma once
#include <cstdint>

template<typename DACType, typename BufferT>
struct DacStream : DACType {
	DacStream() {
	}

	void queue_sample(uint32_t channel, uint32_t value) {
	}

	static inline BufferT dac_buffer;
};
