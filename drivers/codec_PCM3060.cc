/*
 * codec_PCM3060.cc
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

#include "codec_PCM3060.hh"
#include "codec_PCM3060_registers.hh"
#include "stm32xx.h"
#include "util/bitfield.hh"

namespace mdrivlib
{

static constexpr bool DISABLE_I2C = false;

using namespace CodecPCM3060Register;

struct RegisterData {
	uint8_t reg_num;
	uint8_t value;
};

CodecPCM3060::CodecPCM3060(I2CPeriph &i2c, const SaiConfig &saidef)
	: CodecBase{saidef}
	, i2c_(i2c)
	, samplerate_{saidef.samplerate}
	, reset_pin_{saidef.reset_pin, PinMode::Output}
	, I2C_address{static_cast<uint8_t>((CodecPCM3060Register::I2C_BASE_ADDR + (saidef.bus_address & 0b1)) << 1)} {
	reset_pin_.high();
}

CodecPCM3060::Error CodecPCM3060::init() {
	auto err = sai_.init();
	if (err != SaiTdmPeriph::SAI_NO_ERR)
		return CodecPCM3060::I2S_INIT_ERR;

	// reset_pin_.high();

	HAL_Delay(1); // 3846 SYSCLKI cycles = 0.313ms
	return _write_all_registers(samplerate_);
}

uint32_t CodecPCM3060::get_samplerate() {
	return samplerate_;
}

void CodecPCM3060::start() {
	sai_.start();
}

CodecPCM3060::Error CodecPCM3060::_write_all_registers(uint32_t sample_rate) {

	auto sysreg_read = try_read<SystemControl>();
	if (!sysreg_read.has_value())
		return Error::I2C_INIT_ERR;

	// Reset registers
	SystemControl sysreg{
		.DACSingleEnded = 1, .DACPowerSave = 1, .ADCPowerSave = 1, .SystemReset = 1, .ModeRegisterReset = 0};
	if (!write(sysreg))
		return Error::I2C_INIT_ERR;

	HAL_Delay(10);

	//System Reset
	sysreg.ModeRegisterReset = 1;
	sysreg.SystemReset = 0;
	if (!write(sysreg))
		return Error::I2C_INIT_ERR;

	HAL_Delay(10);

	// I2S Format
	DacControl1 dacreg{.Format = DacControl1::Formats::I2S_24bit,
					   .MSInterface = DacControl1::Interfaces::Slave,
					   .ClockSel = DacControl1::ClockSels::SelectPinGroups2};
	if (!write(dacreg))
		return Error::I2C_INIT_ERR;

	AdcControl1 adcreg{.Format = AdcControl1::Formats::I2S_24bit,
					   .MSInterface = AdcControl1::Interfaces::Slave,
					   .ClockSel = AdcControl1::ClockSels::SelectPinGroups1};
	if (!write(adcreg))
		return Error::I2C_INIT_ERR;

	// Turn on ADC and DAC (disable power-save)
	sysreg.ADCPowerSave = 0;
	sysreg.DACPowerSave = 0;
	if (!write(sysreg))
		return Error::I2C_INIT_ERR;

	return CODEC_NO_ERR;
}

} // namespace mdrivlib
