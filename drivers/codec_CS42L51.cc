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
	{INTF_CTL, INTF_CTL_DAC_FORMAT(DAC_DIF_I2S) | INTF_CTL_ADC_I2S},

	// Todo: allow user config of input channels
	{ADC_INPUT, ADC_INPUT_AINA_MUX(MUX_AIN3_MIC) | ADC_INPUT_AINB_MUX(MUX_AIN3_MIC)},
};

CodecCS42L51::CodecCS42L51(I2CPeriph &i2c, const SaiConfig &saidef)
	: CodecBase{saidef}
	, i2c_(i2c)
	, reset_pin_{saidef.reset_pin, PinMode::Output}
	, I2C_address{static_cast<uint8_t>((0x4A + (saidef.bus_address ? 1 : 0)) << 1)} {
	reset_pin_.low();
}

CodecCS42L51::Error CodecCS42L51::init() {
	init_at_samplerate(samplerate_);
	return sai_.init() == SaiTdmPeriph::SAI_NO_ERR ? CODEC_NO_ERR : CODEC_I2C_ERR;
}

void CodecCS42L51::start() {
	sai_.start();

	// Set Power Down bit to 0 after MCLK and LRCK are running (CS42L51 datasheet, section 4.8)
	power_up();
}

CodecCS42L51::Error CodecCS42L51::init_at_samplerate(uint32_t sample_rate) {
	reset_pin_.high();
	HAL_Delay(1);

	// Set Power Down bit to 1 to enter Software Mode (must do it <10ms from reset pin going high).
	// See CS42L51 datasheet, section 4.8
	power_down();

	auto err = _write_samplerate_register(sample_rate);
	if (err != CODEC_NO_ERR)
		return err;
	return _write_all_registers();
}

CodecCS42L51::Error CodecCS42L51::_write_samplerate_register(uint32_t sample_rate) {
	auto sr_mode = _calc_samplerate(sample_rate);
	return _write_register(MIC_POWER_CTL, MIC_POWER_CTL_AUTO | sr_mode);
}

uint8_t CodecCS42L51::_calc_samplerate(uint32_t sample_rate) {
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

CodecCS42L51::Error CodecCS42L51::_write_all_registers() {
	CodecCS42L51::Error err;

	for (auto packet : default_codec_init) {
		err = _write_register(packet.reg_num, packet.value);
		if (err != CODEC_NO_ERR)
			return err;
	}
	return err;
}

CodecCS42L51::Error CodecCS42L51::_write_register(uint8_t reg_address, uint16_t reg_value) {
	uint8_t Byte1 = ((reg_address << 1) & 0xFE) | ((reg_value >> 8) & 0x01);
	uint8_t Byte2 = reg_value & 0xFF;

	if constexpr (DISABLE_I2C)
		return CODEC_NO_ERR;

	auto err = i2c_.mem_write(I2C_address, Byte1, REGISTER_ADDR_SIZE, &Byte2, 1);
	return (err == I2CPeriph::I2C_NO_ERR) ? CODEC_NO_ERR : CODEC_I2C_ERR;
}

CodecCS42L51::Error CodecCS42L51::power_down() {
	return _write_register(POWER_CTL1, POWER_CTL1_PDN);
}

CodecCS42L51::Error CodecCS42L51::power_up() {
	return _write_register(POWER_CTL1, 0);
}
} // namespace mdrivlib

// enum master_slave_mode {
// 	MODE_SLAVE,
// 	MODE_SLAVE_AUTO,
// 	MODE_MASTER,
// };

// static const char * const cs42l51_supply_names[] = {
// 	"VL",
// 	"VD",
// 	"VA",
// 	"VAHP",
// };

// struct cs42l51_private {
// 	unsigned int mclk;
// 	struct clk *mclk_handle;
// 	unsigned int audio_mode;	/* The mode (I2S or left-justified) */
// 	enum master_slave_mode func;
// 	struct regulator_bulk_data supplies[ARRAY_SIZE(cs42l51_supply_names)];
// 	struct gpio_desc *reset_gpio;
// 	struct regmap *regmap;
// };

// #define CS42L51_FORMATS ( \
// 		SNDRV_PCM_FMTBIT_S16_LE  | SNDRV_PCM_FMTBIT_S16_BE  | \
// 		SNDRV_PCM_FMTBIT_S18_3LE | SNDRV_PCM_FMTBIT_S18_3BE | \
// 		SNDRV_PCM_FMTBIT_S20_3LE | SNDRV_PCM_FMTBIT_S20_3BE | \
// 		SNDRV_PCM_FMTBIT_S24_LE  | SNDRV_PCM_FMTBIT_S24_BE)

// static int cs42l51_get_chan_mix(struct snd_kcontrol *kcontrol,
// 			struct snd_ctl_elem_value *ucontrol)
// {
// 	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
// 	unsigned long value = snd_soc_component_read32(component, CS42L51_PCM_MIXER)&3;

// 	switch (value) {
// 	default:
// 	case 0:
// 		ucontrol->value.enumerated.item[0] = 0;
// 		break;
// 	/* same value : (L+R)/2 and (R+L)/2 */
// 	case 1:
// 	case 2:
// 		ucontrol->value.enumerated.item[0] = 1;
// 		break;
// 	case 3:
// 		ucontrol->value.enumerated.item[0] = 2;
// 		break;
// 	}

// 	return 0;
// }

// #define CHAN_MIX_NORMAL	0x00
// #define CHAN_MIX_BOTH	0x55
// #define CHAN_MIX_SWAP	0xFF

// static int cs42l51_set_chan_mix(struct snd_kcontrol *kcontrol,
// 			struct snd_ctl_elem_value *ucontrol)
// {
// 	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
// 	unsigned char val;

// 	switch (ucontrol->value.enumerated.item[0]) {
// 	default:
// 	case 0:
// 		val = CHAN_MIX_NORMAL;
// 		break;
// 	case 1:
// 		val = CHAN_MIX_BOTH;
// 		break;
// 	case 2:
// 		val = CHAN_MIX_SWAP;
// 		break;
// 	}

// 	snd_soc_component_write(component, CS42L51_PCM_MIXER, val);

// 	return 1;
// }

// static const DECLARE_TLV_DB_SCALE(adc_pcm_tlv, -5150, 50, 0);
// static const DECLARE_TLV_DB_SCALE(tone_tlv, -1050, 150, 0);

// static const DECLARE_TLV_DB_SCALE(aout_tlv, -10200, 50, 0);

// static const DECLARE_TLV_DB_SCALE(boost_tlv, 1600, 1600, 0);
// static const DECLARE_TLV_DB_SCALE(adc_boost_tlv, 2000, 2000, 0);
// static const char *chan_mix[] = {
// 	"L R",
// 	"L+R",
// 	"R L",
// };

// static SOC_ENUM_SINGLE_EXT_DECL(cs42l51_chan_mix, chan_mix);

// static const struct snd_kcontrol_new cs42l51_snd_controls[] = {
// 	SOC_DOUBLE_R_SX_TLV("PCM Playback Volume",
// 			CS42L51_PCMA_VOL, CS42L51_PCMB_VOL,
// 			0, 0x19, 0x7F, adc_pcm_tlv),
// 	SOC_DOUBLE_R("PCM Playback Switch",
// 			CS42L51_PCMA_VOL, CS42L51_PCMB_VOL, 7, 1, 1),
// 	SOC_DOUBLE_R_SX_TLV("Analog Playback Volume",
// 			CS42L51_AOUTA_VOL, CS42L51_AOUTB_VOL,
// 			0, 0x34, 0xE4, aout_tlv),
// 	SOC_DOUBLE_R_SX_TLV("ADC Mixer Volume",
// 			CS42L51_ADCA_VOL, CS42L51_ADCB_VOL,
// 			0, 0x19, 0x7F, adc_pcm_tlv),
// 	SOC_DOUBLE_R("ADC Mixer Switch",
// 			CS42L51_ADCA_VOL, CS42L51_ADCB_VOL, 7, 1, 1),
// 	SOC_SINGLE("Playback Deemphasis Switch", CS42L51_DAC_CTL, 3, 1, 0),
// 	SOC_SINGLE("Auto-Mute Switch", CS42L51_DAC_CTL, 2, 1, 0),
// 	SOC_SINGLE("Soft Ramp Switch", CS42L51_DAC_CTL, 1, 1, 0),
// 	SOC_SINGLE("Zero Cross Switch", CS42L51_DAC_CTL, 0, 0, 0),
// 	SOC_DOUBLE_TLV("Mic Boost Volume",
// 			CS42L51_MIC_CTL, 0, 1, 1, 0, boost_tlv),
// 	SOC_DOUBLE_TLV("ADC Boost Volume",
// 		       CS42L51_MIC_CTL, 5, 6, 1, 0, adc_boost_tlv),
// 	SOC_SINGLE_TLV("Bass Volume", CS42L51_TONE_CTL, 0, 0xf, 1, tone_tlv),
// 	SOC_SINGLE_TLV("Treble Volume", CS42L51_TONE_CTL, 4, 0xf, 1, tone_tlv),
// 	SOC_ENUM_EXT("PCM channel mixer",
// 			cs42l51_chan_mix,
// 			cs42l51_get_chan_mix, cs42l51_set_chan_mix),
// };

// /*
//  * to power down, one must:
//  * 1.) Enable the PDN bit
//  * 2.) enable power-down for the select channels
//  * 3.) disable the PDN bit.
//  */
// static int cs42l51_pdn_event(struct snd_soc_dapm_widget *w,
// 		struct snd_kcontrol *kcontrol, int event)
// {
// 	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);

// 	switch (event) {
// 	case SND_SOC_DAPM_PRE_PMD:
// 		snd_soc_component_update_bits(component, CS42L51_POWER_CTL1,
// 				    CS42L51_POWER_CTL1_PDN,
// 				    CS42L51_POWER_CTL1_PDN);
// 		break;
// 	default:
// 	case SND_SOC_DAPM_POST_PMD:
// 		snd_soc_component_update_bits(component, CS42L51_POWER_CTL1,
// 				    CS42L51_POWER_CTL1_PDN, 0);
// 		msleep(20);
// 		break;
// 	}

// 	return 0;
// }

// static const char *cs42l51_dac_names[] = {"Direct PCM",
// 	"DSP PCM", "ADC"};
// static SOC_ENUM_SINGLE_DECL(cs42l51_dac_mux_enum,
// 			    CS42L51_DAC_CTL, 6, cs42l51_dac_names);
// static const struct snd_kcontrol_new cs42l51_dac_mux_controls =
// 	SOC_DAPM_ENUM("Route", cs42l51_dac_mux_enum);

// static const char *cs42l51_adcl_names[] = {"AIN1 Left", "AIN2 Left",
// 	"MIC Left", "MIC+preamp Left"};
// static SOC_ENUM_SINGLE_DECL(cs42l51_adcl_mux_enum,
// 			    CS42L51_ADC_INPUT, 4, cs42l51_adcl_names);
// static const struct snd_kcontrol_new cs42l51_adcl_mux_controls =
// 	SOC_DAPM_ENUM("Route", cs42l51_adcl_mux_enum);

// static const char *cs42l51_adcr_names[] = {"AIN1 Right", "AIN2 Right",
// 	"MIC Right", "MIC+preamp Right"};
// static SOC_ENUM_SINGLE_DECL(cs42l51_adcr_mux_enum,
// 			    CS42L51_ADC_INPUT, 6, cs42l51_adcr_names);
// static const struct snd_kcontrol_new cs42l51_adcr_mux_controls =
// 	SOC_DAPM_ENUM("Route", cs42l51_adcr_mux_enum);

// static const struct snd_soc_dapm_widget cs42l51_dapm_widgets[] = {
// 	SND_SOC_DAPM_SUPPLY("Mic Bias", CS42L51_MIC_POWER_CTL, 1, 1, NULL,
// 			    SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
// 	SND_SOC_DAPM_PGA_E("Left PGA", CS42L51_POWER_CTL1, 3, 1, NULL, 0,
// 		cs42l51_pdn_event, SND_SOC_DAPM_PRE_POST_PMD),
// 	SND_SOC_DAPM_PGA_E("Right PGA", CS42L51_POWER_CTL1, 4, 1, NULL, 0,
// 		cs42l51_pdn_event, SND_SOC_DAPM_PRE_POST_PMD),
// 	SND_SOC_DAPM_ADC_E("Left ADC", "Left HiFi Capture",
// 		CS42L51_POWER_CTL1, 1, 1,
// 		cs42l51_pdn_event, SND_SOC_DAPM_PRE_POST_PMD),
// 	SND_SOC_DAPM_ADC_E("Right ADC", "Right HiFi Capture",
// 		CS42L51_POWER_CTL1, 2, 1,
// 		cs42l51_pdn_event, SND_SOC_DAPM_PRE_POST_PMD),
// 	SND_SOC_DAPM_DAC_E("Left DAC", NULL, CS42L51_POWER_CTL1, 5, 1,
// 			   cs42l51_pdn_event, SND_SOC_DAPM_PRE_POST_PMD),
// 	SND_SOC_DAPM_DAC_E("Right DAC", NULL, CS42L51_POWER_CTL1, 6, 1,
// 			   cs42l51_pdn_event, SND_SOC_DAPM_PRE_POST_PMD),

// 	/* analog/mic */
// 	SND_SOC_DAPM_INPUT("AIN1L"),
// 	SND_SOC_DAPM_INPUT("AIN1R"),
// 	SND_SOC_DAPM_INPUT("AIN2L"),
// 	SND_SOC_DAPM_INPUT("AIN2R"),
// 	SND_SOC_DAPM_INPUT("MICL"),
// 	SND_SOC_DAPM_INPUT("MICR"),

// 	SND_SOC_DAPM_MIXER("Mic Preamp Left",
// 		CS42L51_MIC_POWER_CTL, 2, 1, NULL, 0),
// 	SND_SOC_DAPM_MIXER("Mic Preamp Right",
// 		CS42L51_MIC_POWER_CTL, 3, 1, NULL, 0),

// 	/* HP */
// 	SND_SOC_DAPM_OUTPUT("HPL"),
// 	SND_SOC_DAPM_OUTPUT("HPR"),

// 	/* mux */
// 	SND_SOC_DAPM_MUX("DAC Mux", SND_SOC_NOPM, 0, 0,
// 		&cs42l51_dac_mux_controls),
// 	SND_SOC_DAPM_MUX("PGA-ADC Mux Left", SND_SOC_NOPM, 0, 0,
// 		&cs42l51_adcl_mux_controls),
// 	SND_SOC_DAPM_MUX("PGA-ADC Mux Right", SND_SOC_NOPM, 0, 0,
// 		&cs42l51_adcr_mux_controls),
// };

// static const struct snd_soc_dapm_widget cs42l51_dapm_mclk_widgets[] = {
// 	SND_SOC_DAPM_CLOCK_SUPPLY("MCLK")
// };

// static const struct snd_soc_dapm_route cs42l51_routes[] = {
// 	{"HPL", NULL, "Left DAC"},
// 	{"HPR", NULL, "Right DAC"},

// 	{"Right DAC", NULL, "DAC Mux"},
// 	{"Left DAC", NULL, "DAC Mux"},

// 	{"DAC Mux", "Direct PCM", "Playback"},
// 	{"DAC Mux", "DSP PCM", "Playback"},

// 	{"Left ADC", NULL, "Left PGA"},
// 	{"Right ADC", NULL, "Right PGA"},

// 	{"Mic Preamp Left",  NULL,  "MICL"},
// 	{"Mic Preamp Right", NULL,  "MICR"},

// 	{"PGA-ADC Mux Left",  "AIN1 Left",        "AIN1L" },
// 	{"PGA-ADC Mux Left",  "AIN2 Left",        "AIN2L" },
// 	{"PGA-ADC Mux Left",  "MIC Left",         "MICL"  },
// 	{"PGA-ADC Mux Left",  "MIC+preamp Left",  "Mic Preamp Left" },
// 	{"PGA-ADC Mux Right", "AIN1 Right",       "AIN1R" },
// 	{"PGA-ADC Mux Right", "AIN2 Right",       "AIN2R" },
// 	{"PGA-ADC Mux Right", "MIC Right",        "MICR" },
// 	{"PGA-ADC Mux Right", "MIC+preamp Right", "Mic Preamp Right" },

// 	{"Left PGA", NULL, "PGA-ADC Mux Left"},
// 	{"Right PGA", NULL, "PGA-ADC Mux Right"},
// };

// static int cs42l51_set_dai_fmt(struct snd_soc_dai *codec_dai,
// 		unsigned int format)
// {
// 	struct snd_soc_component *component = codec_dai->component;
// 	struct cs42l51_private *cs42l51 = snd_soc_component_get_drvdata(component);

// 	switch (format & SND_SOC_DAIFMT_FORMAT_MASK) {
// 	case SND_SOC_DAIFMT_I2S:
// 	case SND_SOC_DAIFMT_LEFT_J:
// 	case SND_SOC_DAIFMT_RIGHT_J:
// 		cs42l51->audio_mode = format & SND_SOC_DAIFMT_FORMAT_MASK;
// 		break;
// 	default:
// 		dev_err(component->dev, "invalid DAI format\n");
// 		return -EINVAL;
// 	}

// 	switch (format & SND_SOC_DAIFMT_MASTER_MASK) {
// 	case SND_SOC_DAIFMT_CBM_CFM:
// 		cs42l51->func = MODE_MASTER;
// 		break;
// 	case SND_SOC_DAIFMT_CBS_CFS:
// 		cs42l51->func = MODE_SLAVE_AUTO;
// 		break;
// 	default:
// 		dev_err(component->dev, "Unknown master/slave configuration\n");
// 		return -EINVAL;
// 	}

// 	return 0;
// }

// struct cs42l51_ratios {
// 	unsigned int ratio;
// 	unsigned char speed_mode;
// 	unsigned char mclk;
// };

// static struct cs42l51_ratios slave_ratios[] = {
// 	{  512, CS42L51_QSM_MODE, 0 }, {  768, CS42L51_QSM_MODE, 0 },
// 	{ 1024, CS42L51_QSM_MODE, 0 }, { 1536, CS42L51_QSM_MODE, 0 },
// 	{ 2048, CS42L51_QSM_MODE, 0 }, { 3072, CS42L51_QSM_MODE, 0 },
// 	{  256, CS42L51_HSM_MODE, 0 }, {  384, CS42L51_HSM_MODE, 0 },
// 	{  512, CS42L51_HSM_MODE, 0 }, {  768, CS42L51_HSM_MODE, 0 },
// 	{ 1024, CS42L51_HSM_MODE, 0 }, { 1536, CS42L51_HSM_MODE, 0 },
// 	{  128, CS42L51_SSM_MODE, 0 }, {  192, CS42L51_SSM_MODE, 0 },
// 	{  256, CS42L51_SSM_MODE, 0 }, {  384, CS42L51_SSM_MODE, 0 },
// 	{  512, CS42L51_SSM_MODE, 0 }, {  768, CS42L51_SSM_MODE, 0 },
// 	{  128, CS42L51_DSM_MODE, 0 }, {  192, CS42L51_DSM_MODE, 0 },
// 	{  256, CS42L51_DSM_MODE, 0 }, {  384, CS42L51_DSM_MODE, 0 },
// };

// static struct cs42l51_ratios slave_auto_ratios[] = {
// 	{ 1024, CS42L51_QSM_MODE, 0 }, { 1536, CS42L51_QSM_MODE, 0 },
// 	{ 2048, CS42L51_QSM_MODE, 1 }, { 3072, CS42L51_QSM_MODE, 1 },
// 	{  512, CS42L51_HSM_MODE, 0 }, {  768, CS42L51_HSM_MODE, 0 },
// 	{ 1024, CS42L51_HSM_MODE, 1 }, { 1536, CS42L51_HSM_MODE, 1 },
// 	{  256, CS42L51_SSM_MODE, 0 }, {  384, CS42L51_SSM_MODE, 0 },
// 	{  512, CS42L51_SSM_MODE, 1 }, {  768, CS42L51_SSM_MODE, 1 },
// 	{  128, CS42L51_DSM_MODE, 0 }, {  192, CS42L51_DSM_MODE, 0 },
// 	{  256, CS42L51_DSM_MODE, 1 }, {  384, CS42L51_DSM_MODE, 1 },
// };

// /*
//  * Master mode mclk/fs ratios.
//  * Recommended configurations are SSM for 4-50khz and DSM for 50-100kHz ranges
//  * The table below provides support of following ratios:
//  * 128: SSM (%128) with div2 disabled
//  * 256: SSM (%128) with div2 enabled
//  * In both cases, if sampling rate is above 50kHz, SSM is overridden
//  * with DSM (%128) configuration
//  */
// static struct cs42l51_ratios master_ratios[] = {
// 	{ 128, CS42L51_SSM_MODE, 0 }, { 256, CS42L51_SSM_MODE, 1 },
// };

// static int cs42l51_set_dai_sysclk(struct snd_soc_dai *codec_dai,
// 		int clk_id, unsigned int freq, int dir)
// {
// 	struct snd_soc_component *component = codec_dai->component;
// 	struct cs42l51_private *cs42l51 = snd_soc_component_get_drvdata(component);

// 	cs42l51->mclk = freq;
// 	return 0;
// }

// static int cs42l51_hw_params(struct snd_pcm_substream *substream,
// 		struct snd_pcm_hw_params *params,
// 		struct snd_soc_dai *dai)
// {
// 	struct snd_soc_component *component = dai->component;
// 	struct cs42l51_private *cs42l51 = snd_soc_component_get_drvdata(component);
// 	int ret;
// 	unsigned int i;
// 	unsigned int rate;
// 	unsigned int ratio;
// 	struct cs42l51_ratios *ratios = NULL;
// 	int nr_ratios = 0;
// 	int intf_ctl, power_ctl, fmt, mode;

// 	switch (cs42l51->func) {
// 	case MODE_MASTER:
// 		ratios = master_ratios;
// 		nr_ratios = ARRAY_SIZE(master_ratios);
// 		break;
// 	case MODE_SLAVE:
// 		ratios = slave_ratios;
// 		nr_ratios = ARRAY_SIZE(slave_ratios);
// 		break;
// 	case MODE_SLAVE_AUTO:
// 		ratios = slave_auto_ratios;
// 		nr_ratios = ARRAY_SIZE(slave_auto_ratios);
// 		break;
// 	}

// 	/* Figure out which MCLK/LRCK ratio to use */
// 	rate = params_rate(params);     /* Sampling rate, in Hz */
// 	ratio = cs42l51->mclk / rate;    /* MCLK/LRCK ratio */
// 	for (i = 0; i < nr_ratios; i++) {
// 		if (ratios[i].ratio == ratio)
// 			break;
// 	}

// 	if (i == nr_ratios) {
// 		/* We did not find a matching ratio */
// 		dev_err(component->dev, "could not find matching ratio\n");
// 		return -EINVAL;
// 	}

// 	intf_ctl = snd_soc_component_read32(component, CS42L51_INTF_CTL);
// 	power_ctl = snd_soc_component_read32(component, CS42L51_MIC_POWER_CTL);

// 	intf_ctl &= ~(CS42L51_INTF_CTL_MASTER | CS42L51_INTF_CTL_ADC_I2S
// 			| CS42L51_INTF_CTL_DAC_FORMAT(7));
// 	power_ctl &= ~(CS42L51_MIC_POWER_CTL_SPEED(3)
// 			| CS42L51_MIC_POWER_CTL_MCLK_DIV2);

// 	switch (cs42l51->func) {
// 	case MODE_MASTER:
// 		intf_ctl |= CS42L51_INTF_CTL_MASTER;
// 		mode = ratios[i].speed_mode;
// 		/* Force DSM mode if sampling rate is above 50kHz */
// 		if (rate > 50000)
// 			mode = CS42L51_DSM_MODE;
// 		power_ctl |= CS42L51_MIC_POWER_CTL_SPEED(mode);
// 		/*
// 		 * Auto detect mode is not applicable for master mode and has to
// 		 * be disabled. Otherwise SPEED[1:0] bits will be ignored.
// 		 */
// 		power_ctl &= ~CS42L51_MIC_POWER_CTL_AUTO;
// 		break;
// 	case MODE_SLAVE:
// 		power_ctl |= CS42L51_MIC_POWER_CTL_SPEED(ratios[i].speed_mode);
// 		break;
// 	case MODE_SLAVE_AUTO:
// 		power_ctl |= CS42L51_MIC_POWER_CTL_AUTO;
// 		break;
// 	}

// 	switch (cs42l51->audio_mode) {
// 	case SND_SOC_DAIFMT_I2S:
// 		intf_ctl |= CS42L51_INTF_CTL_ADC_I2S;
// 		intf_ctl |= CS42L51_INTF_CTL_DAC_FORMAT(CS42L51_DAC_DIF_I2S);
// 		break;
// 	case SND_SOC_DAIFMT_LEFT_J:
// 		intf_ctl |= CS42L51_INTF_CTL_DAC_FORMAT(CS42L51_DAC_DIF_LJ24);
// 		break;
// 	case SND_SOC_DAIFMT_RIGHT_J:
// 		switch (params_width(params)) {
// 		case 16:
// 			fmt = CS42L51_DAC_DIF_RJ16;
// 			break;
// 		case 18:
// 			fmt = CS42L51_DAC_DIF_RJ18;
// 			break;
// 		case 20:
// 			fmt = CS42L51_DAC_DIF_RJ20;
// 			break;
// 		case 24:
// 			fmt = CS42L51_DAC_DIF_RJ24;
// 			break;
// 		default:
// 			dev_err(component->dev, "unknown format\n");
// 			return -EINVAL;
// 		}
// 		intf_ctl |= CS42L51_INTF_CTL_DAC_FORMAT(fmt);
// 		break;
// 	default:
// 		dev_err(component->dev, "unknown format\n");
// 		return -EINVAL;
// 	}

// 	if (ratios[i].mclk)
// 		power_ctl |= CS42L51_MIC_POWER_CTL_MCLK_DIV2;

// 	ret = snd_soc_component_write(component, CS42L51_INTF_CTL, intf_ctl);
// 	if (ret < 0)
// 		return ret;

// 	ret = snd_soc_component_write(component, CS42L51_MIC_POWER_CTL, power_ctl);
// 	if (ret < 0)
// 		return ret;

// 	return 0;
// }

// static int cs42l51_dai_mute(struct snd_soc_dai *dai, int mute)
// {
// 	struct snd_soc_component *component = dai->component;
// 	int reg;
// 	int mask = CS42L51_DAC_OUT_CTL_DACA_MUTE|CS42L51_DAC_OUT_CTL_DACB_MUTE;

// 	reg = snd_soc_component_read32(component, CS42L51_DAC_OUT_CTL);

// 	if (mute)
// 		reg |= mask;
// 	else
// 		reg &= ~mask;

// 	return snd_soc_component_write(component, CS42L51_DAC_OUT_CTL, reg);
// }

// static int cs42l51_of_xlate_dai_id(struct snd_soc_component *component,
// 				   struct device_node *endpoint)
// {
// 	/* return dai id 0, whatever the endpoint index */
// 	return 0;
// }

// static const struct snd_soc_dai_ops cs42l51_dai_ops = {
// 	.hw_params      = cs42l51_hw_params,
// 	.set_sysclk     = cs42l51_set_dai_sysclk,
// 	.set_fmt        = cs42l51_set_dai_fmt,
// 	.digital_mute   = cs42l51_dai_mute,
// };

// static struct snd_soc_dai_driver cs42l51_dai = {
// 	.name = "cs42l51-hifi",
// 	.playback = {
// 		.stream_name = "Playback",
// 		.channels_min = 1,
// 		.channels_max = 2,
// 		.rates = SNDRV_PCM_RATE_8000_96000,
// 		.formats = CS42L51_FORMATS,
// 	},
// 	.capture = {
// 		.stream_name = "Capture",
// 		.channels_min = 1,
// 		.channels_max = 2,
// 		.rates = SNDRV_PCM_RATE_8000_96000,
// 		.formats = CS42L51_FORMATS,
// 	},
// 	.ops = &cs42l51_dai_ops,
// };

// static int cs42l51_component_probe(struct snd_soc_component *component)
// {
// 	int ret, reg;
// 	struct snd_soc_dapm_context *dapm;
// 	struct cs42l51_private *cs42l51;

// 	cs42l51 = snd_soc_component_get_drvdata(component);
// 	dapm = snd_soc_component_get_dapm(component);

// 	if (cs42l51->mclk_handle)
// 		snd_soc_dapm_new_controls(dapm, cs42l51_dapm_mclk_widgets, 1);

// 	/*
// 	 * DAC configuration
// 	 * - Use signal processor
// 	 * - auto mute
// 	 * - vol changes immediate
// 	 * - no de-emphasize
// 	 */
// 	reg = CS42L51_DAC_CTL_DATA_SEL(1)
// 		| CS42L51_DAC_CTL_AMUTE | CS42L51_DAC_CTL_DACSZ(0);
// 	ret = snd_soc_component_write(component, CS42L51_DAC_CTL, reg);
// 	if (ret < 0)
// 		return ret;

// 	return 0;
// }

// static const struct snd_soc_component_driver soc_component_device_cs42l51 = {
// 	.probe			= cs42l51_component_probe,
// 	.controls		= cs42l51_snd_controls,
// 	.num_controls		= ARRAY_SIZE(cs42l51_snd_controls),
// 	.dapm_widgets		= cs42l51_dapm_widgets,
// 	.num_dapm_widgets	= ARRAY_SIZE(cs42l51_dapm_widgets),
// 	.dapm_routes		= cs42l51_routes,
// 	.num_dapm_routes	= ARRAY_SIZE(cs42l51_routes),
// 	.of_xlate_dai_id	= cs42l51_of_xlate_dai_id,
// 	.idle_bias_on		= 1,
// 	.use_pmdown_time	= 1,
// 	.endianness		= 1,
// 	.non_legacy_dai_naming	= 1,
// };

// static bool cs42l51_writeable_reg(struct device *dev, unsigned int reg)
// {
// 	switch (reg) {
// 	case CS42L51_POWER_CTL1:
// 	case CS42L51_MIC_POWER_CTL:
// 	case CS42L51_INTF_CTL:
// 	case CS42L51_MIC_CTL:
// 	case CS42L51_ADC_CTL:
// 	case CS42L51_ADC_INPUT:
// 	case CS42L51_DAC_OUT_CTL:
// 	case CS42L51_DAC_CTL:
// 	case CS42L51_ALC_PGA_CTL:
// 	case CS42L51_ALC_PGB_CTL:
// 	case CS42L51_ADCA_ATT:
// 	case CS42L51_ADCB_ATT:
// 	case CS42L51_ADCA_VOL:
// 	case CS42L51_ADCB_VOL:
// 	case CS42L51_PCMA_VOL:
// 	case CS42L51_PCMB_VOL:
// 	case CS42L51_BEEP_FREQ:
// 	case CS42L51_BEEP_VOL:
// 	case CS42L51_BEEP_CONF:
// 	case CS42L51_TONE_CTL:
// 	case CS42L51_AOUTA_VOL:
// 	case CS42L51_AOUTB_VOL:
// 	case CS42L51_PCM_MIXER:
// 	case CS42L51_LIMIT_THRES_DIS:
// 	case CS42L51_LIMIT_REL:
// 	case CS42L51_LIMIT_ATT:
// 	case CS42L51_ALC_EN:
// 	case CS42L51_ALC_REL:
// 	case CS42L51_ALC_THRES:
// 	case CS42L51_NOISE_CONF:
// 	case CS42L51_CHARGE_FREQ:
// 		return true;
// 	default:
// 		return false;
// 	}
// }

// static bool cs42l51_volatile_reg(struct device *dev, unsigned int reg)
// {
// 	switch (reg) {
// 	case CS42L51_STATUS:
// 		return true;
// 	default:
// 		return false;
// 	}
// }

// static bool cs42l51_readable_reg(struct device *dev, unsigned int reg)
// {
// 	switch (reg) {
// 	case CS42L51_CHIP_REV_ID:
// 	case CS42L51_POWER_CTL1:
// 	case CS42L51_MIC_POWER_CTL:
// 	case CS42L51_INTF_CTL:
// 	case CS42L51_MIC_CTL:
// 	case CS42L51_ADC_CTL:
// 	case CS42L51_ADC_INPUT:
// 	case CS42L51_DAC_OUT_CTL:
// 	case CS42L51_DAC_CTL:
// 	case CS42L51_ALC_PGA_CTL:
// 	case CS42L51_ALC_PGB_CTL:
// 	case CS42L51_ADCA_ATT:
// 	case CS42L51_ADCB_ATT:
// 	case CS42L51_ADCA_VOL:
// 	case CS42L51_ADCB_VOL:
// 	case CS42L51_PCMA_VOL:
// 	case CS42L51_PCMB_VOL:
// 	case CS42L51_BEEP_FREQ:
// 	case CS42L51_BEEP_VOL:
// 	case CS42L51_BEEP_CONF:
// 	case CS42L51_TONE_CTL:
// 	case CS42L51_AOUTA_VOL:
// 	case CS42L51_AOUTB_VOL:
// 	case CS42L51_PCM_MIXER:
// 	case CS42L51_LIMIT_THRES_DIS:
// 	case CS42L51_LIMIT_REL:
// 	case CS42L51_LIMIT_ATT:
// 	case CS42L51_ALC_EN:
// 	case CS42L51_ALC_REL:
// 	case CS42L51_ALC_THRES:
// 	case CS42L51_NOISE_CONF:
// 	case CS42L51_STATUS:
// 	case CS42L51_CHARGE_FREQ:
// 		return true;
// 	default:
// 		return false;
// 	}
// }

// const struct regmap_config cs42l51_regmap = {
// 	.reg_bits = 8,
// 	.reg_stride = 1,
// 	.val_bits = 8,
// 	.use_single_write = true,
// 	.readable_reg = cs42l51_readable_reg,
// 	.volatile_reg = cs42l51_volatile_reg,
// 	.writeable_reg = cs42l51_writeable_reg,
// 	.max_register = CS42L51_CHARGE_FREQ,
// 	.cache_type = REGCACHE_RBTREE,
// };
// EXPORT_SYMBOL_GPL(cs42l51_regmap);

// int cs42l51_probe(struct device *dev, struct regmap *regmap)
// {
// 	struct cs42l51_private *cs42l51;
// 	unsigned int val;
// 	int ret, i;

// 	if (IS_ERR(regmap))
// 		return PTR_ERR(regmap);

// 	cs42l51 = devm_kzalloc(dev, sizeof(struct cs42l51_private),
// 			       GFP_KERNEL);
// 	if (!cs42l51)
// 		return -ENOMEM;

// 	dev_set_drvdata(dev, cs42l51);
// 	cs42l51->regmap = regmap;

// 	cs42l51->mclk_handle = devm_clk_get(dev, "MCLK");
// 	if (IS_ERR(cs42l51->mclk_handle)) {
// 		if (PTR_ERR(cs42l51->mclk_handle) != -ENOENT)
// 			return PTR_ERR(cs42l51->mclk_handle);
// 		cs42l51->mclk_handle = NULL;
// 	}

// 	for (i = 0; i < ARRAY_SIZE(cs42l51->supplies); i++)
// 		cs42l51->supplies[i].supply = cs42l51_supply_names[i];

// 	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(cs42l51->supplies),
// 				      cs42l51->supplies);
// 	if (ret != 0) {
// 		dev_err(dev, "Failed to request supplies: %d\n", ret);
// 		return ret;
// 	}

// 	ret = regulator_bulk_enable(ARRAY_SIZE(cs42l51->supplies),
// 				    cs42l51->supplies);
// 	if (ret != 0) {
// 		dev_err(dev, "Failed to enable supplies: %d\n", ret);
// 		return ret;
// 	}

// 	cs42l51->reset_gpio = devm_gpiod_get_optional(dev, "reset",
// 						      GPIOD_OUT_LOW);
// 	if (IS_ERR(cs42l51->reset_gpio))
// 		return PTR_ERR(cs42l51->reset_gpio);

// 	if (cs42l51->reset_gpio) {
// 		dev_dbg(dev, "Release reset gpio\n");
// 		gpiod_set_value_cansleep(cs42l51->reset_gpio, 0);
// 		mdelay(2);
// 	}

// 	/* Verify that we have a CS42L51 */
// 	ret = regmap_read(regmap, CS42L51_CHIP_REV_ID, &val);
// 	if (ret < 0) {
// 		dev_err(dev, "failed to read I2C\n");
// 		goto error;
// 	}

// 	if ((val != CS42L51_MK_CHIP_REV(CS42L51_CHIP_ID, CS42L51_CHIP_REV_A)) &&
// 	    (val != CS42L51_MK_CHIP_REV(CS42L51_CHIP_ID, CS42L51_CHIP_REV_B))) {
// 		dev_err(dev, "Invalid chip id: %x\n", val);
// 		ret = -ENODEV;
// 		goto error;
// 	}
// 	dev_info(dev, "Cirrus Logic CS42L51, Revision: %02X\n",
// 		 val & CS42L51_CHIP_REV_MASK);

// 	ret = devm_snd_soc_register_component(dev,
// 			&soc_component_device_cs42l51, &cs42l51_dai, 1);
// 	if (ret < 0)
// 		goto error;

// 	return 0;

// error:
// 	regulator_bulk_disable(ARRAY_SIZE(cs42l51->supplies),
// 			       cs42l51->supplies);
// 	return ret;
// }
