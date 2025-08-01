#pragma once
#include "drivers/interrupt.hh"
#include "drivers/sai_config_struct.hh"
#include "drivers/sai_tdm.hh"
#include <array>
#include <span>

namespace mdrivlib
{

class CodecBase {
	using CallbackT = Interrupt::ISRType;

public:
	CodecBase(const SaiConfig &sai_def)
		: sai_{sai_def}
		, samplerate_{sai_def.samplerate} {
	}

	template<typename T>
	void set_tx_buffer(std::span<T> tx_buf, uint32_t frames_per_block) {
		sai_.set_tx_buffer(reinterpret_cast<uint8_t *>(tx_buf.data()), frames_per_block);
	}

	template<typename T>
	void set_rx_buffer(std::span<T> rx_buf, uint32_t frames_per_block) {
		sai_.set_rx_buffer(reinterpret_cast<uint8_t *>(rx_buf.data()), frames_per_block);
	}

	template<typename T>
	void set_tx_buffer_start(std::span<T> tx_buf) {
		constexpr uint32_t BytesPerDMAXfer = 4; //must match DMA MemAlign
		sai_.set_tx_buffer_start(reinterpret_cast<uint8_t *>(tx_buf.data()), tx_buf.size() / BytesPerDMAXfer);
	}

	template<typename T>
	void set_rx_buffer_start(std::span<T> buf) {
		constexpr uint32_t BytesPerDMAXfer = 4; //must match DMA MemAlign
		sai_.set_rx_buffer_start(reinterpret_cast<uint8_t *>(buf.data()), buf.size() / BytesPerDMAXfer);
	}

	// Take any type which decays to an address (C-style array)
	void set_tx_buffer_start(auto &buf) {
		set_tx_buffer_start(std::span{buf, sizeof(buf)});
	}
	void set_rx_buffer_start(auto &buf) {
		set_rx_buffer_start(std::span{buf, sizeof(buf)});
	}

	// Take a std::array
	template<typename T, size_t NumFramesInBuffer>
	void set_tx_buffer_start(std::array<T, NumFramesInBuffer> &buf) {
		set_tx_buffer_start(std::span{buf.data(), sizeof(buf)});
	}
	template<typename T, size_t NumFramesInBuffer>
	void set_rx_buffer_start(std::array<T, NumFramesInBuffer> &buf) {
		set_rx_buffer_start(std::span{buf.data(), sizeof(buf)});
	}

	void set_callbacks(CallbackT &&tx_complete_cb, CallbackT &&tx_half_complete_cb) {
		sai_.set_callbacks(std::forward<CallbackT>(tx_complete_cb), std::forward<CallbackT>(tx_half_complete_cb));
	}

	enum Error {
		CODEC_NO_ERR = 0,
		CODEC_I2C_ERR,
		I2C_INIT_ERR,
		I2S_INIT_ERR,
	};

	Error change_samplerate_blocksize(uint32_t sample_rate, uint32_t block_size) {
		samplerate_ = sample_rate;

		if (sai_.change_samplerate_blocksize(sample_rate, block_size) == SaiTdmPeriph::SAI_NO_ERR) {
			return CODEC_NO_ERR;
		} else {
			return I2S_INIT_ERR;
		}
	}

	uint32_t get_samplerate() {
		return samplerate_;
	}

	uint32_t get_sai_errors() {
		uint32_t errs = std::min<uint32_t>(sai_.fe_errors, 0xFF);
		errs <<= 8;
		errs += std::min<uint32_t>(sai_.te_errors, 0xFF);
		errs <<= 8;
		errs += std::min<uint32_t>(sai_.dme_errors, 0xFF);
		return errs;
	}

protected:
	SaiTdmPeriph sai_;
	uint32_t samplerate_;
};

} // namespace mdrivlib
