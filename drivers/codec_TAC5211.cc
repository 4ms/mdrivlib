/*
 * codec_TAC5211.cc
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

#include "codec_TAC5211.hh"
#include "codec_TAC5211_registers.hh"

namespace mdrivlib
{
using namespace CodecTAC5211Register;

// Wake up the device by writing to P0_R2 to disable sleep mode
// Wait for at least 2ms to allow the device to complete the internal wake-up sequence
// Override the default configuration registers or programmable coefficients value as required
// Enable all desired input channels by writing to P0_R118
// Enable all desired audio serial interface input/output channels by writing to P0_R40 to P0_R47 for DAC
//   and P0_R30 to P0_R37 for ADC
// Power-up the ADC, DAC and MICBIAS by writing to P0_R120

constexpr std::array<Registers, 7> default_init{
	Page{.page = 0},

	// SwReset{.Reset = 1},

	DeviceMiscConfig{.WakeFromSleep = 1},

	Config0{
		.BusErrorRecovery = Config0::ResumeAfterBusErr,
		.BusErrorDetection = Config0::DontDetectBusErr,
		.SlotLength = Config0::Bits24,
		.Format = Config0::I2S,
	},

	ClockConfig2{
		.AllowFractionalPLL = 1,
		.DisablePLL = 0,
	},

	PowerConfig{
		.MicBiasPowered = 1,
		.DacPowered = 1,
		.AdcPowered = 1,
	},

	IntFConfig1{
		.DoutDrive = 0b1,  // active low, active high
		.DoutSel = 0b0101, // primary DOUT
	},

	/*
00 = 00 01 = 00 02 = 01 03 = 00 04 = 00 05 = 15 06 = 35 07 = 00
08 = 00 09 = 00 0a = 32 0b = 00 0c = 00 0d = 00 0e = 00 0f = 00
10 = 51 11 = 80 12 = 00 13 = 00 14 = 00 15 = 00 16 = 00 17 = 00
18 = 40 19 = 00 1a = 70 1b = 00 1c = 00 1d = 00 1e = 20 1f = 21
20 = 02 21 = 03 22 = 04 23 = 05 24 = 06 25 = 07 26 = 00 27 = 00
28 = 20 29 = 21 2a = 02 2b = 03 2c = 04 2d = 05 2e = 06 2f = 07
30 = 00 31 = 00 32 = 00 33 = 00 34 = 40 35 = 00 36 = 00 37 = 20
38 = 00 39 = 00 3a = 00 3b = 00 3c = 00 3d = 10 3e = 50 3f = 00
40 = 00 41 = 40 42 = 00 43 = 54 44 = 00 45 = 00 46 = 00 47 = 00
48 = 00 49 = 00 4a = 00 4b = 00 4c = 2e 4d = 00 4e = 00 4f = 00
50 = 00 51 = 00 52 = a1 53 = 80 54 = 00 55 = 00 56 = 00 57 = a1
58 = 80 59 = 00 5a = 00 5b = a1 5c = 80 5d = 00 5e = 00 5f = a1
60 = 80 61 = 00 62 = 00 63 = 00 64 = 20 65 = 20 66 = 20 67 = c9
68 = 80 69 = c9 6a = 80 6b = 20 6c = 20 6d = 20 6e = c9 6f = 80
70 = c9 71 = 80 72 = 18 73 = 18 74 = 00 75 = 00 76 = cc 77 = 00
78 = 06 79 = 80 7a = e0 7b = 00 7c = c0 7d = 10 7e = 24
	   */
};

CodecTAC5211::CodecTAC5211(I2CPeriph &i2c, const SaiConfig &saidef)
	: CodecBase{saidef}
	, i2c_(i2c)
	, samplerate_{saidef.samplerate}
	, I2C_address{static_cast<uint8_t>((I2C_BASE_ADDR + ((saidef.bus_address & 0b11) << 1)))} {
}

CodecTAC5211::Error CodecTAC5211::init() {
	auto err = sai_.init();
	if (err != SaiTdmPeriph::SAI_NO_ERR)
		return CodecTAC5211::I2S_INIT_ERR;

	return init_registers(samplerate_);
}

CodecTAC5211::Error CodecTAC5211::change_samplerate_blocksize(uint32_t sample_rate, uint32_t block_size) {
	samplerate_ = sample_rate;

	if (sai_.change_samplerate_blocksize(sample_rate, block_size) == SaiTdmPeriph::SAI_NO_ERR) {
		return CodecTAC5211::CODEC_NO_ERR;
	} else {
		return CodecTAC5211::I2S_INIT_ERR;
	}
}

uint32_t CodecTAC5211::get_samplerate() {
	return samplerate_;
}

void CodecTAC5211::start() {
	sai_.start();
}

void CodecTAC5211::stop() {
	sai_.stop();
	sai_.init();
}

uint32_t CodecTAC5211::get_sai_errors() {
	uint32_t errs = std::min<uint32_t>(sai_.fe_errors, 0xFF);
	errs <<= 8;
	errs += std::min<uint32_t>(sai_.te_errors, 0xFF);
	errs <<= 8;
	errs += std::min<uint32_t>(sai_.dme_errors, 0xFF);
	return errs;
}

CodecTAC5211::Error CodecTAC5211::init_registers(uint32_t sample_rate) {
	using namespace CodecTAC5211Register;

	HAL_Delay(2);
	write<DeviceMiscConfig>({.WakeFromSleep = 1});
	HAL_Delay(2);

	for (auto reg : default_init) {

		if (!std::visit([this](auto r) -> bool { return write(r); }, reg))
			return Error::CODEC_I2C_ERR;
	}

	return Error::CODEC_NO_ERR;
}

void CodecTAC5211::dump_registers() {
	HAL_Delay(100);
	for (auto i = 0u; i < 127; i++) {
		uint8_t val;
		auto err = i2c_.mem_read(I2C_address, i, 1, &val, 1);
		if (err == I2CPeriph::Error::I2C_NO_ERR)
			printf("%02x = %02x\t", i, val);
		else
			printf("%02x: ERR %d\n", i, err);

		if ((i % 8) == 7)
			printf("\n");
	}
	printf("\n");
}

} // namespace mdrivlib
