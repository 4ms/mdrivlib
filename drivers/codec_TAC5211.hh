/*
 * TAC5211
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
#include <cstdint>

namespace mdrivlib
{

class CodecTAC5211 : public CodecBase {
public:
	CodecTAC5211(I2CPeriph &i2c, const SaiConfig &saidef);

	Error init();
	void start();
	void stop();
	void dump_registers();

private:
	I2CPeriph &i2c_;

	Error init_registers(uint32_t sample_rate);

	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c_.write_reg(I2C_address, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	template<typename Reg>
	Reg read() {
		static bool got_error_once = false;
		auto reg = i2c_.read_reg<Reg>(I2C_address);

		if (!reg.has_value() && !got_error_once) {
			pr_dbg("Error reading Reg 0x%x\n", Reg::Address);
			got_error_once = true;
		}
		return reg.value_or(Reg::make(0));
	}

	const uint8_t I2C_address;
	static inline constexpr auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};
} // namespace mdrivlib
