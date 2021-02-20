#pragma once
#include <cstdint>

template<typename DACType, typename BufferT>
struct DacStream : DACType {
	DacStream() {
		dac_buffer.set_head(dac_buffer.capacity() / 2);
	}

	void queue_sample(uint32_t value) {
		dac_buffer.put(value);
	}

	void output_next(uint32_t chan) {
		this->set_output_blocking(chan, dac_buffer.get());
	}

	static inline BufferT dac_buffer;
};
