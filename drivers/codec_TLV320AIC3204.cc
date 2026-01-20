/*
 * codec_TLV320AIC3204.cc
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

#include "codec_TLV320AIC3204.hh"
#include "codec_TLV320AIC3204_registers.hh"

namespace mdrivlib
{
using namespace CodecTLV320AIC3204Register;

constexpr std::array<Registers, 1> default_init{
	{
		Page{.page = 0},
	},
};

CodecTLV320AIC3204::CodecTLV320AIC3204(I2CPeriph &i2c, const SaiConfig &saidef)
	: CodecBase{saidef}
	, i2c_(i2c)
	, reset_pin_{saidef.reset_pin, PinMode::Output}
	, I2C_address{static_cast<uint8_t>(I2C_BASE_ADDR << 1)} {
	reset_pin_.high();
}

CodecTLV320AIC3204::Error CodecTLV320AIC3204::init() {
	auto err = sai_.init();
	if (err != SaiTdmPeriph::SAI_NO_ERR)
		return CodecTLV320AIC3204::I2S_INIT_ERR;

	reset_pin_.high();

	// Datasheet section 3.1:
	// To initialize the device to the default operation condition, the hardware reset pin (RESET) must be pulled low for at least 10ns.

	// After the TLV320AIC3204 initializes through hardware reset at power-up or software reset, the internal
	// registers initialize to default values. This initialization takes place within 1ms after pulling the RESET
	// signal high. During this initialization phase, no register-read or register-write operation should be
	// performed on ADC or DAC coefficient buffers. Also, no block within the codec should be powered up
	// during the initialization phase

	reset_pin_.low();
	HAL_Delay(1);
	reset_pin_.high();
	HAL_Delay(2);

	return init_registers(samplerate_);
}

void CodecTLV320AIC3204::start() {
	sai_.start();
}

void CodecTLV320AIC3204::stop() {
	sai_.stop();
	sai_.init();
}

CodecTLV320AIC3204::Error CodecTLV320AIC3204::init_registers(uint32_t sample_rate) {
	using namespace CodecTLV320AIC3204Register;

	// From datasheet sec 3.1:
	// ...subsequently the device can be reset via software reset. Writing ‘1’ into Page 0, Register 1, D(0) resets the device.

	HAL_Delay(2);
	write<Page>({.page = 0});
	write<SwReset>({.Reset = 1});
	HAL_Delay(2);

	for (auto reg : default_init) {
		if (!std::visit([this](auto r) -> bool { return write(r); }, reg))
			return Error::CODEC_I2C_ERR;
	}

	return Error::CODEC_NO_ERR;
}

void CodecTLV320AIC3204::dump_registers() {
	// HAL_Delay(100);
	// for (auto i = 0u; i < 127; i++) {
	// 	uint8_t val;
	// 	auto err = i2c_.mem_read(I2C_address, i, 1, &val, 1);
	// 	if (err == I2CPeriph::Error::I2C_NO_ERR)
	// 		printf("%02x = %02x\t", i, val);
	// 	else
	// 		printf("%02x: ERR %d\n", i, err);

	// 	if ((i % 8) == 7)
	// 		printf("\n");
	// }
	// printf("\n");
}

} // namespace mdrivlib
