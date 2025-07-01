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
#include "codec_PCM3060_registers.hh"
#include "i2c.hh"
#include <span>

namespace mdrivlib
{

class CodecPCM3060 : public CodecBase {
public:
	CodecPCM3060(I2CPeriph &i2c, const SaiConfig &saidef);

	Error init();
	Error init(const std::span<const CodecPCM3060Register::AnyRegister> init_regs);
	void start();
	void stop();

private:
	I2CPeriph &i2c_;
	uint32_t samplerate_;
	Pin reset_pin_;

	Error _write_all_registers(const std::span<const CodecPCM3060Register::AnyRegister> init_regs,
							   uint32_t sample_rate);

	const uint8_t I2C_address;

	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c_.write_reg(I2C_address, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	// Returns 0 if failed
	template<typename Reg>
	Reg read() {
		auto reg = i2c_.read_reg<Reg>(I2C_address);
		return reg.value_or(Reg::make(0));
	}

	template<typename Reg>
	std::optional<Reg> try_read() {
		return i2c_.read_reg<Reg>(I2C_address);
	}
};
} // namespace mdrivlib
