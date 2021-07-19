/*
 * codec_i2c.c
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

#include "codec_CS42L51.hh"
#include "codec_CS42L51_registers.h"
#include "stm32xx.h"

static constexpr bool DISABLE_I2C = false;

namespace mdrivlib
{
using namespace CodecCS42L51Register;

struct RegisterData {
	uint8_t reg_num;
	uint8_t value;
};

static RegisterData default_codec_init[] = {
	// {POWER_CTL1, 0},
	{MIC_POWER_CTL, MIC_POWER_CTL_AUTO},
	{INTF_CTL, INTF_CTL_DAC_FORMAT(DAC_DIF_I2S) | INTF_CTL_ADC_I2S},

	//... etc
};

CodecCS42L51::CodecCS42L51(I2CPeriph &i2c, const SaiConfig &saidef, PinNoInit reset_pin, uint8_t address_bit)
	: i2c_(i2c)
	, sai_{saidef}
	, samplerate_{saidef.samplerate}
	, reset_pin_{reset_pin, PinMode::Output}
	, CODEC_ADDRESS{static_cast<uint8_t>(0x4A + (address_bit ? 1 : 0))} {
	reset_pin_.low();
}

void CodecCS42L51::init() {
	init_at_samplerate(samplerate_);
	sai_.init();
}

void CodecCS42L51::set_txrx_buffers(uint8_t *tx_buf_ptr, uint8_t *rx_buf_ptr, uint32_t block_size) {
	sai_.set_txrx_buffers(tx_buf_ptr, rx_buf_ptr, block_size);
}

void CodecCS42L51::set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb) {
	sai_.set_callbacks(std::move(tx_complete_cb), std::move(tx_half_complete_cb));
}

uint32_t CodecCS42L51::get_samplerate() {
	return samplerate_;
}

void CodecCS42L51::start() {
	sai_.start();

	// Set Power Down bit to 0 after MCLK and LRCK are running (CS42L51 datasheet, section 4.8)
	power_up();
}

CodecCS42L51::Error CodecCS42L51::init_at_samplerate(uint32_t sample_rate) {
	reset_pin_.high();
	HAL_Delay(1);

	// Set Power Down bit to 1 to enter Software Mode (must do it <10ms from reset pin going high). See CS42L51
	// datasheet, section 4.8
	power_down();

	return _write_all_registers(sample_rate);
}

CodecCS42L51::Error CodecCS42L51::_write_all_registers(uint32_t sample_rate) {
	CodecCS42L51::Error err;

	for (auto packet : default_codec_init) {
		err = _write_register(packet.reg_num, packet.value);
		if (err != CODEC_NO_ERR)
			return err;
	}
	return err;
}

auto CodecCS42L51::_calc_samplerate(uint32_t sample_rate) {
	if (sample_rate > 50000)
		return DSM_MODE;
	else if (sample_rate == 16000)
		return HSM_MODE;
	else if (sample_rate == 8000)
		return QSM_MODE;
	else if (sample_rate > 4000)
		return SSM_MODE;
	else
		return (uint8_t)0;
}

CodecCS42L51::Error CodecCS42L51::_write_register(uint8_t reg_address, uint16_t reg_value) {
	uint8_t Byte1 = ((reg_address << 1) & 0xFE) | ((reg_value >> 8) & 0x01);
	uint8_t Byte2 = reg_value & 0xFF;
	uint8_t data[2] = {Byte1, Byte2};

	if constexpr (DISABLE_I2C)
		return CODEC_NO_ERR;

	auto err = i2c_.write(CODEC_ADDRESS, data, 2);
	// auto err = i2c_.mem_write(CODEC_ADDRESS, Byte1, REGISTER_ADDR_SIZE, &Byte2, 1);
	return (err == I2CPeriph::I2C_NO_ERR) ? CODEC_NO_ERR : CODEC_I2C_ERR;
}

CodecCS42L51::Error CodecCS42L51::power_down() {
	return _write_register(POWER_CTL1, POWER_CTL1_PDN);
}

CodecCS42L51::Error CodecCS42L51::power_up() {
	return _write_register(POWER_CTL1, 0);
}
} // namespace mdrivlib
