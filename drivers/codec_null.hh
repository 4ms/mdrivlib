#pragma once
#include "codec.hh"
#include <cstdint>

namespace mdrivlib
{

struct CodecNull : CodecBase {
	static inline const SaiConfig saidef{};

	CodecNull()
		: CodecBase(saidef) {
	}

	Error init() {
		return Error::I2C_INIT_ERR;
	}
	Error change_samplerate_blocksize(uint32_t sample_rate, uint32_t block_size) {
		return Error::I2C_INIT_ERR;
	}
	uint32_t get_samplerate() {
		return 0;
	}
	void start() {
	}
	void stop() {
	}

	uint32_t get_sai_errors() {
		return 0;
	}
};
} // namespace mdrivlib
