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
		: sai_{sai_def} {
	}

	template<typename T>
	void set_tx_buffer_start(std::span<T> tx_buf) {
		constexpr uint32_t BytesPerDMAXfer = 4; //must match DMA MemAlign
		sai_.set_tx_buffer_start(reinterpret_cast<uint8_t *>(tx_buf.data()), tx_buf.size_bytes() / BytesPerDMAXfer);
	}

	template<typename T>
	void set_rx_buffer_start(std::span<T> buf) {
		constexpr uint32_t BytesPerDMAXfer = 4; //must match DMA MemAlign
		sai_.set_rx_buffer_start(reinterpret_cast<uint8_t *>(buf.data()), buf.size_bytes() / BytesPerDMAXfer);
	}

	void set_tx_buffer_start(auto &buf) {
		set_tx_buffer_start(std::span{buf, sizeof(buf)});
	}

	void set_rx_buffer_start(auto &buf) {
		set_rx_buffer_start(std::span{buf, sizeof(buf)});
	}

	template<typename T, size_t NumFramesInBuffer>
	void set_tx_buffer_start(std::array<T, NumFramesInBuffer> &tx_buf) {
		constexpr uint32_t BytesPerDMAXfer = 4; //must match DMA MemAlign
		constexpr uint32_t block_size = sizeof(tx_buf) / BytesPerDMAXfer;
		sai_.set_tx_buffer_start(reinterpret_cast<uint8_t *>(tx_buf.data()), block_size);
	}

	template<typename T, size_t BUFSIZE>
	void set_rx_buffer_start(std::array<T, BUFSIZE> &rx_buf) {
		constexpr uint32_t BytesPerDMAXfer = 4; //must match DMA MemAlign
		constexpr uint32_t block_size = sizeof(rx_buf) / BytesPerDMAXfer;
		sai_.set_rx_buffer_start(reinterpret_cast<uint8_t *>(rx_buf.data()), block_size);
	}

	void set_callbacks(CallbackT &&tx_complete_cb, CallbackT &&tx_half_complete_cb) {
		sai_.set_callbacks(std::forward<CallbackT>(tx_complete_cb), std::forward<CallbackT>(tx_half_complete_cb));
	}

protected:
	SaiTdmPeriph sai_;
};

} // namespace mdrivlib
