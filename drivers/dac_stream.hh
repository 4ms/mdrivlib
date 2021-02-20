#pragma once
#include <cstdint>

template<typename DACType, typename BufferT>
struct DacStream : DACType {
	DacStream() {
		for (unsigned i = 0; i < NumChips; i++)
			dac_buffer[i].set_head(dac_buffer[i].capacity() / 2);
	}

	void queue_sample(uint32_t chip, uint32_t value) {
		dac_buffer[chip].put(value);
	}

	void output_next() {
		this->end_xfer(cur_chip);
		if (cur_chip == NumChips - 1) {
			cur_chip = 0;
			this->latch.low();
		} else
			cur_chip++;
		this->prepare_xfer(cur_chip, dac_buffer[cur_chip].get());
		this->latch.high();
		this->do_xfer();
	}

	static constexpr unsigned NumChips = DACType::SpiConf::NumChips;

	static inline BufferT dac_buffer[NumChips];
	uint32_t cur_chip = 0;
};
