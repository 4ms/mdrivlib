/*
 * CodecWM8731.c
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

#include "drivers/codec.hh"
#include "drivers/i2c.hh"
#include <cstdint>

namespace mdrivlib
{

class CodecWM8731 : public CodecBase {
public:
	CodecWM8731(I2CPeriph &i2c, const SaiConfig &saidef);

	Error init();
	void start();

	Error init_at_samplerate(uint32_t sample_rate);
	Error power_down();

private:
	I2CPeriph &i2c_;

	Error _write_register(uint8_t RegisterAddr, uint16_t RegisterValue);
	Error _write_all_registers(uint32_t sample_rate);
	Error _reset();
	uint16_t _calc_samplerate(uint32_t sample_rate);

	const uint8_t I2C_address; // I2C_BASE_ADDRESS = 0b10000000;
	const static inline auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};

} // namespace mdrivlib
