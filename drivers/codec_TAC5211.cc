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

constexpr std::array<Registers, 11> default_init{
	Page{.page = 0},

	DeviceMiscConfig{
		.WakeFromSleep = 1,
		.SleepExitEnableVref = 1,
	},

	Config0{
		.SlotLength = Bits24,
		.Format = I2S,
	},

	VrefMicBiasConfig{
		.VrefFullScale = VrefMicBiasConfig::Vref2750mV,
	},

	AdcChan1Config{
		.Bandwidth = AdcChan1Config::Audio24kHz,
		.FullScaleValue = AdcChan1Config::Diff2Vrms,
		.InSource = AdcChan1Config::Single,
	},

	Out1Config0{
		.Vcom = Out1Config0::Vref_div_0_6,
		.RoutingConfig = Out1Config0::Diff,
		.OutSource = Out1Config0::DAC,
	},

	Out1Config1{
		.LevelOut1P = Out1Config1::dB0,
		.DriveOut1P = Out1Config1::LineOut300,
	},

	Out1Config2{
		.LevelOut1M = Out1Config2::dB0,
		.DriveOut1M = Out1Config2::LineOut300,
	},

	ChannelEnable{
		.OutChan2Enable = 1,
		.OutChan1Enable = 1,
		.InChan2Enable = 1,
		.InChan1Enable = 1,
	},

	PowerConfig{
		.MicBiasPowered = 0,
		.DacPowered = 1,
		.AdcPowered = 1,
	},

	IntFConfig1{
		.DoutDrive = 0b1,  // active low, active high
		.DoutSel = 0b0101, // primary DOUT
	},

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

	// From datasheet:
	// Wake up the device by writing to P0_R2 to disable sleep mode
	// Wait for at least 2ms to allow the device to complete the internal wake-up sequence

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
