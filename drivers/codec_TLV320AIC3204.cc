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

constexpr std::array<Registers, 42> default_init{
	{
		// PAGE 0
		//////////////////////////////////
		PageSelect{.page = 0},

		SwReset{.Reset = 1},

		//
		// PIN MUX
		//

		// MCLK pin -> Codec clock input (B1)
		ClockSettingMultiplexors{.CodecClkIn = ClockSettingMultiplexors::MCLK,
								 .PLLInputClock = ClockSettingMultiplexors::PLL_MCLK,
								 .HighPLLClockRange = 0},

		// BCLK->I2S BCLK (C2)
		// WCLK->I2S WCLK (E3)
		AudioInterfaceSetting1{.DOUTHighImpedance = 0,
							   .WCLKDirection = AudioInterfaceSetting1::Input,
							   .BCLKDirection = AudioInterfaceSetting1::Input,
							   .WordLength = AudioInterfaceSetting1::Bits32,
							   .Interface = AudioInterfaceSetting1::I2S},

		// DOUT->I2S DOUT (J5)
		DOUTControl{.DOUTFunction = DOUTControl::DOUT_PrimaryDOUT},

		// DIN->I2S DIN (I4)
		DINControl{.DINFunction = DINControl::DIN_Enabled},

		// Try:
		// MFP3(TP12)->HP Detect (W6) P0_R56 D2-D1=00, P0_R67 D7=1

		//
		// ADC Setup
		//

		// Program and power up NADC and MADC
		// CODEC_CLKIN = NADC * MADC * AOSR * ADC_FS, note: AOSR is set next (128)
		// NADC should be as large as possible as long as the following condition can still be met:
		// MADC * AOSR / 32 ≥ RC, where RC is 6 or 8 in Table 2-7
		// MADC * 128 / 32 >= 8, MADC >= 2
		// Choose: NADC = 128, MADC = 2 => CODEC_CLKIN = 1 * 2 * 128 * 48000 = 12.288MHz
		ClockSettingNADCValues{.NADCValue = 1, .NADCPowerUp = 1},
		ClockSettingMADCValues{.MADCValue = 2, .MADCPowerUp = 1},

		// Program OSR value (AOSR)
		// Filter A with AOSR of 128 should be used for 48kHz high performance operation.
		// Filter B with AOSR of 64 should be used for 96kHz operations
		ADCOversampling{.AOSR = ADCOversampling::AOSR128},

		// Program the processing block to be used
		// Select ADC PRB_R1
		ADCProcessingBlock{.ProcessingBlock = ADCProcessingBlock::PRB_R1},

		// NDAC/MDAC
		ClockSettingNDACValues{.NDACValue = 1, .NDACPowerUp = 1},
		ClockSettingMDACValues{.MDACValue = 2, .MDACPowerUp = 1},

		// DOSR = 128
		DACOSRSettingMSB{.DOSRMSB = 0},
		DACOSRSettingLSB{.DOSRLSB = 0x80},

		// DAC PB
		DACProcessingBlock{.ProcessingBlock = DACProcessingBlock::PRB_P8}, //??

		// Program Analog Blocks

		// PAGE 1
		//////////////////////////////////

		PageSelect{.page = 1},

		// 	Disable coarse AVdd generation
		PowerConfiguration{.DisableWeakAVDDtoDVDD = 1},

		// 	Enable Master Analog Power Control
		LDOControl{.AVDDLDOPowerUp = 1, .AnalogBlocksDisabled = 0},

		// 	Program Common Mode voltage
		CommonModeControl{.FullChipCommonMode = CommonModeControl::CM_0_9V},

		// 	Program PowerTune (PTM) mode R4=highest
		ADCPowerTuneConfiguration{.PTMConfiguration = ADCPowerTuneConfiguration::PTM_R4},

		// 	Program MicPGA startup delay
		AnalogInputQuickCharging{.PowerUpTime = AnalogInputQuickCharging::Default},

		// 	Program Reference fast charging
		ReferencePowerUpConfiguration{.Configuration = ReferencePowerUpConfiguration::Auto40ms},

		// 	Routing of inputs and common mode to ADC input
		LeftMICPGAPositiveRouting{.IN1LtoLeftMICPGA = InputRoutings::Routed_20k},
		LeftMICPGANegativeRouting{.CM1LtoLeftMICPGA = InputRoutings::Routed_20k},
		RightMICPGAPositiveRouting{.IN1RtoRightMICPGA = InputRoutings::Routed_20k},
		RightMICPGANegativeRouting{.CM1RtoRightMICPGA = InputRoutings::Routed_20k},

		// 	Unmute analog PGAs and set analog gain
		LeftMICPGAVolume{.Volume = 0xc},
		RightMICPGAVolume{.Volume = 0xc},

		LOLRoutingSelection{.LeftDACtoLOL = 1},
		LORRoutingSelection{.RightDACtoLOR = 1},
		PlaybackConfiguration1{.LeftDACPTM = PlaybackConfiguration1::PTM_P3_P4},
		PlaybackConfiguration2{.RightDACPTM = PlaybackConfiguration1::PTM_P3_P4},

		LOLDriverGain{.Gain = 0x0, .Mute = 0},
		LORDriverGain{.Gain = 0x0, .Mute = 0},

		OutputDriverPowerControl{
			.MARPowerUp = 0, .MALPowerUp = 0, .LORPowerUp = 1, .LOLPowerUp = 1, .HPRPowerUp = 0, .HPLPowerUp = 0},

		// PAGE 0
		//////////////////////////////////

		PageSelect{.page = 0},

		// 	Power up ADC Channels
		ADCChannelSetup{.RightADCPowerUp = 1, .LeftADCPowerUp = 1},

		// 	Unmute digital volume control
		ADCFineGainAdjust{.RightADCMute = 0, .LeftADCMute = 0},

		DACChannelSetup1{.DACSoftStep = DACChannelSetup1::SoftStepDisabled,
						 .RightDACDataPath = DACChannelSetup1::DACNormal,
						 .LeftDACDataPath = DACChannelSetup1::DACNormal,
						 .RightDACPowerUp = 1,
						 .LeftDACPowerUp = 1},

		DACChannelSetup2{.DACMasterVolumeControl = DACChannelSetup2::Independent,
						 .RightDACMute = 0,
						 .LeftDACMute = 0,
						 .DACAutoMute = DACChannelSetup2::AutoMuteDisabled},

		LeftDACVolume{.Volume = 0x0},  //0 = 0dB
		RightDACVolume{.Volume = 0x0}, //0 = 0dB
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
	write<PageSelect>({.page = 0});
	write<SwReset>({.Reset = 1});
	HAL_Delay(2);

	for (auto reg : default_init) {
		auto ok = std::visit([this](auto r) -> bool {
			if constexpr (std::is_base_of_v<BusReg::WriteAccess, decltype(r)>)
				return write(r);
			else
				return false;
		}, reg);

		if (!ok)
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
