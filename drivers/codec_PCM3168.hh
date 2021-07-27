/*
 * CodecPCM3168.cc
 *
 * Author: Dan Green (danngreen1@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * See http://creativecommons.org/licenses/MIT/ for more information.
 *
 * -----------------------------------------------------------------------------
 */

#pragma once

#include "codec.hh"
#include "i2c.hh"
#include "sai_tdm.hh"
#include <cstdint>

namespace mdrivlib
{

class CodecPCM3168 {
public:
	enum Error {
		CODEC_NO_ERR = 0,
		CODEC_I2C_ERR,
		I2C_INIT_ERR,
		I2S_CLK_INIT_ERR,
		I2STX_INIT_ERR,
		I2SRX_INIT_ERR,
		I2STX_DMA_INIT_ERR,
		I2SRX_DMA_INIT_ERR,
		I2STX_XMIT_DMA_ERR,
		I2SRX_XMIT_DMA_ERR,
		SAI_DMA_IT_FE,
		SAI_DMA_IT_TE,
		SAI_DMA_IT_DME,
		INVALID_PARAM
	};

	CodecPCM3168(I2CPeriph &i2c, const SaiConfig &saidef);

	void init();

	template<typename FrameT, size_t BUFSIZE>
	void set_tx_buffers(std::array<FrameT, BUFSIZE> tx_buf) {
		constexpr uint32_t block_size = sizeof(tx_buf) / 2;
		sai_.set_tx_buffers(reinterpret_cast<uint8_t *>(tx_buf.data()), block_size);
		// static_assert(block_size == 1024);
	}

	template<typename FrameT, size_t BUFSIZE>
	void set_rx_buffers(std::array<FrameT, BUFSIZE> rx_buf) {
		constexpr uint32_t block_size = sizeof(rx_buf) / 2;
		sai_.set_rx_buffers(reinterpret_cast<uint8_t *>(rx_buf.data()), block_size);
		// static_assert(block_size == 768);
	}

	void set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb);

	void start();
	uint32_t get_samplerate();
	Error init_at_samplerate(uint32_t sample_rate);
	Error power_down();
	Error power_up();

private:
	I2CPeriph &i2c_;
	SaiTdmPeriph sai_;
	uint32_t samplerate_;
	Pin reset_pin_;

	Error _write_register(uint8_t RegisterAddr, uint8_t RegisterValue);
	Error _write_all_registers(uint32_t sample_rate);
	auto _calc_samplerate(uint32_t sample_rate);

	const uint8_t I2C_address;
	static inline constexpr auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};
} // namespace mdrivlib
