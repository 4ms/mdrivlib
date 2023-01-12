#pragma once
#include "drivers/interrupt.hh"
#include "drivers/sai_config_struct.hh"
#include "drivers/sai_tdm.hh"
#include <array>
#include <cstdint>

namespace mdrivlib
{

class CodecBase {
	using CallbackT = Interrupt::ISRType;

public:
	CodecBase(const SaiConfig &sai_def)
		: sai_{sai_def} {
	}

	template<typename FrameT, size_t NumFramesInBuffer>
	void set_tx_buffer_start(std::array<FrameT, NumFramesInBuffer> &tx_buf) {
		// template<typename FrameT>
		// void set_tx_buffer_start(std::span<FrameT> tx_buf) {
		//FIXME: This equation is wrong but result is right:
		//??? size_bytes(buffer) / 2
		// sizeof(tx_buf) == tx_buf.size() * sizeof(FrameT)
		// ??block_size = tx_buf.size() * sizeof(FrameT)[==8] / NumHalfTransfers[==2]
		// ??block_size = (tx_buf.size() / 2) * 8 = Number of Frames * 4
		// ??the var is not "block_size", it's "number_data_transfers"
		// ??which equals Number of 32-bit words transferred
		// which equals NumberofFrames in buffer * Number of channels * bytes per channel / 4
		// = Number of Frames * 2
		constexpr uint32_t NumHalfTransfers = 2;
		constexpr uint32_t BytesPerSample = 4;
		constexpr uint32_t block_size = sizeof(tx_buf) * NumHalfTransfers / BytesPerSample;
		sai_.set_tx_buffer_start(reinterpret_cast<uint8_t *>(tx_buf.data()), block_size);
	}

	template<typename FrameT, size_t BUFSIZE>
	void set_rx_buffer_start(std::array<FrameT, BUFSIZE> &rx_buf) {
		// template<typename FrameT>
		// void set_rx_buffer_start(std::span<FrameT> rx_buf) {
		constexpr uint32_t NumHalfTransfers = 2;
		constexpr uint32_t BytesPerSample = 4;
		constexpr uint32_t block_size = sizeof(rx_buf) * NumHalfTransfers / BytesPerSample;
		sai_.set_rx_buffer_start(reinterpret_cast<uint8_t *>(rx_buf.data()), block_size);
	}

	void set_callbacks(CallbackT &&tx_complete_cb, CallbackT &&tx_half_complete_cb) {
		sai_.set_callbacks(std::forward<CallbackT>(tx_complete_cb), std::forward<CallbackT>(tx_half_complete_cb));
	}

protected:
	SaiTdmPeriph sai_;
};

} // namespace mdrivlib
