#pragma once
#include <array>
#include <variant>

#include "drivers/bus_register.hh"

namespace mdrivlib::CodecTLV320AIC3204Register
{
using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

// Datasheet:
// https://www.ti.com/lit/ds/symlink/tlv320aic3204.pdf
// Application reference:
// https://www.ti.com/lit/ml/slaa557/slaa557.pdf

// Un-shifted address: 0x18
// Shifted: 0x30 for write, 0x31 for read
// Datasheet section 9.3.8.1
constexpr inline uint8_t I2C_BASE_ADDR = 0b0011000;

struct PageSelect : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x00;

	uint8_t page;
};

struct SwReset : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x01;

	uint8_t Reset : 1;
	uint8_t : 7;
};

// Registers 2-3: Reserved

struct ClockSettingMultiplexors : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x04;

	uint8_t CodecClkIn : 2;
	enum CodecClkIns { MCLK = 0b00, BCLK = 0b01, GPIO = 0b10, PLL = 0b11 };

	uint8_t PLLInputClock : 2;
	enum PLLInputClocks { PLL_MCLK = 0b00, PLL_BCLK = 0b01, PLL_GPIO = 0b10, PLL_DIN = 0b11 };

	uint8_t : 2;

	uint8_t HighPLLClockRange : 1;

	uint8_t : 1;
};

struct ClockSettingPLLPandRValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x05;

	uint8_t PLLDividerR : 4;
	enum PLLDividerRs { R1 = 0b0001, R2, R3, R4 };

	uint8_t PLLDividerP : 3;
	enum PLLDividerPs { P8 = 0b000, P1 = 0b001, P2, P3, P4, P5, P6, P7 };

	uint8_t PLLPowerUp : 1;
};

struct ClockSettingPLLJValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x06;

	uint8_t PLLDividerJ : 6;
	enum PLLDividerRs { J4 = 0b000100, J5, J6, /*...*/ J63 = 0b111111 };

	uint8_t PLLPowerUp : 2;
};

struct ClockSettingPLLDValuesMSB : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x07;

	uint8_t PLLDividerDMSB : 6;
	uint8_t : 2;
};

struct ClockSettingPLLDValuesLSB : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x08;

	uint8_t PLLDividerDLSB;
};

// Registers 9-10 (0x09-0x0A): Reserved

struct ClockSettingNDACValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x0B;

	uint8_t NDACValue : 7;
	uint8_t NDACPowerUp : 1;
};

struct ClockSettingMDACValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x0C;

	uint8_t MDACValue : 7;
	uint8_t MDACPowerUp : 1;
};

struct DACOSRSettingMSB : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x0D;

	uint8_t DOSRMSB : 2;
	uint8_t : 6;
};

struct DACOSRSettingLSB : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x0E;

	uint8_t DOSRLSB;
};

// Registers 15-17 (0x0F-0x11): Reserved

struct ClockSettingNADCValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x12;

	uint8_t NADCValue : 7;
	uint8_t NADCPowerUp : 1;
};

struct ClockSettingMADCValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x13;

	uint8_t MADCValue : 7;
	uint8_t MADCPowerUp : 1;
};

struct ADCOversampling : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x14;

	uint8_t AOSR;
	enum AOSRs { AOSR256 = 0, AOSR32 = 32, AOSR64 = 64, AOSR128 = 128 };
};

// Registers 21-24 (0x15-0x18): Reserved

struct ClockSettingCDIVCLKIN : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x19;

	uint8_t CDIVClkIn : 3;
	enum CDIVClkIns {
		CDIV_MCLK = 0b000,
		CDIV_BCLK,
		CDIV_DIN,
		CDIV_PLL_CLK,
		CDIV_DAC_CLK,
		CDIV_DAC_MOD_CLK,
		CDIV_ADC_CLK,
		CDIV_ADC_MOD_CLK,
	};

	uint8_t : 5;
};

struct ClockSettingCLKOUTMDiv : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x1A;

	uint8_t CLKOUTMDivValue : 7;
	uint8_t CLKOUTMDivPowerUp : 1;
};

struct AudioInterfaceSetting1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x1B;

	uint8_t DOUTHighImpedance : 1;
	uint8_t : 1;
	uint8_t WCLKDirection : 1;
	uint8_t BCLKDirection : 1;

	uint8_t WordLength : 2;
	enum WordLengths { Bits16 = 0b00, Bits20 = 0b01, Bits24 = 0b10, Bits32 = 0b11 };

	uint8_t Interface : 2;
	enum Interfaces { I2S = 0b00, DSP = 0b01, RJF = 0b10, LJF = 0b11 };
};

struct AudioInterfaceSetting2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x1C;

	uint8_t DataOffset;
};

struct AudioInterfaceSetting3 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x1D;

	uint8_t BDIVClkIn : 2;
	enum BDIVClkIns { BDIV_DAC_CLK = 0b00, BDIV_DAC_MOD_CLK = 0b01, BDIV_ADC_CLK = 0b10, BDIV_ADC_MOD_CLK = 0b11 };

	uint8_t BCLKWCLKPowerControl : 1;
	uint8_t BCLKPolarity : 1;
	uint8_t LoopbackADCToDAC : 1;
	uint8_t LoopbackDINToDOUT : 1;
	uint8_t : 2;
};

struct ClockSettingBCLKNDiv : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x1E;

	uint8_t BCLKNDivValue : 7;
	uint8_t BCLKNDivPowerUp : 1;
};

struct AudioInterfaceSetting4 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x1F;

	uint8_t SecondaryDataInputMux : 1;
	enum SecondaryDataInputMuxs { SecDIN_GPIO = 0, SecDIN_SCLK = 1 };

	uint8_t ADCWordClockMux : 2;
	enum ADCWordClockMuxs { ADCWCLK_GPIO = 0b00, ADCWCLK_SCLK = 0b01, ADCWCLK_MISO = 0b10 };

	uint8_t SecondaryWordClockMux : 2;
	enum SecondaryWordClockMuxs { SecWCLK_GPIO = 0b00, SecWCLK_SCLK = 0b01, SecWCLK_MISO = 0b10, SecWCLK_DOUT = 0b11 };

	uint8_t SecondaryBitClockMux : 2;
	enum SecondaryBitClockMuxs { SecBCLK_GPIO = 0b00, SecBCLK_SCLK = 0b01, SecBCLK_MISO = 0b10, SecBCLK_DOUT = 0b11 };

	uint8_t : 1;
};

struct AudioInterfaceSetting5 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x20;

	uint8_t AudioDataInControl : 1;
	uint8_t ADCWordClockControl : 1;
	uint8_t WordClockControl : 1;
	uint8_t BitClockControl : 1;
	uint8_t : 4;
};

struct AudioInterfaceSetting6 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x21;

	uint8_t SecondaryDataOutControl : 1;
	uint8_t PrimaryDataOutControl : 1;

	uint8_t SecondaryWordClockOutControl : 2;
	enum SecondaryWordClockOutControls { SecWCLKOut_WCLK = 0b00, SecWCLKOut_DAC_FS = 0b01, SecWCLKOut_ADC_FS = 0b10 };

	uint8_t WCLKOutControl : 2;
	enum WCLKOutControls { WCLKOut_DAC_FS = 0b00, WCLKOut_ADC_FS = 0b01, WCLKOut_SecWCLK = 0b10 };

	uint8_t SecondaryBitClockOutControl : 1;
	uint8_t BCLKOutControl : 1;
};

struct DigitalInterfaceMiscSetting : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x22;

	uint8_t : 5;
	uint8_t I2CGeneralCallAddress : 1;
	uint8_t : 2;
};

// Register 35 (0x23): Reserved

struct ADCFlagRegister : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x24;

	uint8_t : 1;
	uint8_t RightAGCGainSaturated : 1;
	uint8_t RightADCPoweredUp : 1;
	uint8_t RightADCPGAGainApplied : 1;
	uint8_t : 1;
	uint8_t LeftAGCGainSaturated : 1;
	uint8_t LeftADCPoweredUp : 1;
	uint8_t LeftADCPGAGainApplied : 1;
};

struct DACFlagRegister1 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x25;

	uint8_t : 1;
	uint8_t HPRPoweredUp : 1;
	uint8_t LORPoweredUp : 1;
	uint8_t RightDACPoweredUp : 1;
	uint8_t : 1;
	uint8_t HPLPoweredUp : 1;
	uint8_t LOLPoweredUp : 1;
	uint8_t LeftDACPoweredUp : 1;
};

struct DACFlagRegister2 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x26;

	uint8_t RightDACPGAGainApplied : 1;
	uint8_t : 3;
	uint8_t LeftDACPGAGainApplied : 1;
	uint8_t : 3;
};

// Registers 39-41 (0x27-0x29): Reserved

struct StickyFlagRegister1 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x2A;

	uint8_t : 2;
	uint8_t RightADCOverflow : 1;
	uint8_t LeftADCOverflow : 1;
	uint8_t : 2;
	uint8_t RightDACOverflow : 1;
	uint8_t LeftDACOverflow : 1;
};

struct InterruptFlagRegister1 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x2B;

	uint8_t : 2;
	uint8_t RightADCOverflow : 1;
	uint8_t LeftADCOverflow : 1;
	uint8_t : 2;
	uint8_t RightDACOverflow : 1;
	uint8_t LeftDACOverflow : 1;
};

struct StickyFlagRegister2 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x2C;

	uint8_t : 2;
	uint8_t RightDRCSignalThreshold : 1;
	uint8_t LeftDRCSignalThreshold : 1;
	uint8_t HeadsetInsertionRemoval : 1;
	uint8_t HeadsetButtonPress : 1;
	uint8_t HPROverCurrent : 1;
	uint8_t HPLOverCurrent : 1;
};

struct StickyFlagRegister3 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x2D;

	uint8_t : 1;
	uint8_t RightADCDCMeasurementAvailable : 1;
	uint8_t LeftADCDCMeasurementAvailable : 1;
	uint8_t : 2;
	uint8_t RightAGCNoiseThreshold : 1;
	uint8_t LeftAGCNoiseThreshold : 1;
	uint8_t : 1;
};

struct InterruptFlagRegister2 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x2E;

	uint8_t : 2;
	uint8_t RightDRCSignalThreshold : 1;
	uint8_t LeftDRCSignalThreshold : 1;
	uint8_t HeadsetInsertionDetect : 1;
	uint8_t HeadsetButtonPress : 1;
	uint8_t HPROverCurrent : 1;
	uint8_t HPLOverCurrent : 1;
};

struct InterruptFlagRegister3 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x2F;

	uint8_t : 1;
	uint8_t RightADCDCMeasurementAvailable : 1;
	uint8_t LeftADCDCMeasurementAvailable : 1;
	uint8_t : 2;
	uint8_t RightAGCNoiseThreshold : 1;
	uint8_t LeftAGCNoiseThreshold : 1;
	uint8_t : 1;
};

struct INT1InterruptControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x30;

	uint8_t INT1PulseControl : 1;
	uint8_t INT1DCMeasurement : 1;
	uint8_t INT1Overflow : 1;
	uint8_t INT1OverCurrent : 1;
	uint8_t INT1AGCNoise : 1;
	uint8_t INT1DRCSignalThreshold : 1;
	uint8_t INT1ButtonPress : 1;
	uint8_t INT1HeadsetInsertion : 1;
};

struct INT2InterruptControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x31;

	uint8_t INT2PulseControl : 1;
	uint8_t INT2DCMeasurement : 1;
	uint8_t INT2Overflow : 1;
	uint8_t INT2OverCurrent : 1;
	uint8_t INT2AGCNoise : 1;
	uint8_t INT2DRCSignalThreshold : 1;
	uint8_t INT2ButtonPress : 1;
	uint8_t INT2HeadsetInsertion : 1;
};

// Registers 50-51 (0x32-0x33): Reserved

struct GPIOControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x34;

	uint8_t GPIOOutputValue : 1;
	uint8_t GPIOInputValue : 1;

	uint8_t GPIOFunction : 4;
	enum GPIOFunctions {
		GPIO_Disabled = 0b0000,
		GPIO_SecondaryInput = 0b0001,
		GPIO_GeneralPurposeInput = 0b0010,
		GPIO_GeneralPurposeOutput = 0b0011,
		GPIO_CLKOUT = 0b0100,
		GPIO_INT1 = 0b0101,
		GPIO_INT2 = 0b0110,
		GPIO_ADC_WCLK = 0b0111,
		GPIO_SecondaryBCLK = 0b1000,
		GPIO_SecondaryWCLK = 0b1001,
		GPIO_DigMicClock = 0b1010,
	};

	uint8_t : 2;
};

struct DOUTControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x35;

	uint8_t DOUTOutputValue : 1;

	uint8_t DOUTFunction : 3;
	enum DOUTFunctions {
		DOUT_Disabled = 0b000,
		DOUT_PrimaryDOUT = 0b001,
		DOUT_GeneralPurposeOutput = 0b010,
		DOUT_CLKOUT = 0b011,
		DOUT_INT1 = 0b100,
		DOUT_INT2 = 0b101,
		DOUT_SecondaryBCLK = 0b110,
		DOUT_SecondaryWCLK = 0b111,
	};

	uint8_t DOUTBusKeeperDisabled : 1;
	uint8_t : 3;
};

struct DINControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x36;

	uint8_t DINInputValue : 1;

	uint8_t DINFunction : 2;
	enum DINFunctions { DIN_Disabled = 0b00, DIN_Enabled = 0b01, DIN_GeneralPurposeInput = 0b10 };

	uint8_t : 5;
};

struct MISOControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x37;

	uint8_t MISOOutputValue : 1;

	uint8_t MISOFunction : 4;
	enum MISOFunctions {
		MISO_Disabled = 0b0000,
		MISO_SPI = 0b0001,
		MISO_GeneralPurposeOutput = 0b0010,
		MISO_CLKOUT = 0b0011,
		MISO_INT1 = 0b0100,
		MISO_INT2 = 0b0101,
		MISO_ADC_WCLK = 0b0110,
		MISO_DigMicClock = 0b0111,
		MISO_SecondaryDataOut = 0b1000,
		MISO_SecondaryBCLK = 0b1001,
		MISO_SecondaryWCLK = 0b1010,
	};

	uint8_t : 3;
};

struct SCLKControl : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x38;

	uint8_t SCLKInputValue : 1;

	uint8_t SCLKFunction : 2;
	enum SCLKFunctions { SCLK_Disabled = 0b00, SCLK_Enabled = 0b01, SCLK_GeneralPurposeInput = 0b10 };

	uint8_t : 5;
};

// Registers 57-59 (0x39-0x3B): Reserved

struct DACProcessingBlock : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x3C;

	uint8_t ProcessingBlock : 5;
	enum ProcessingBlocks {
		PRB_P1 = 1,
		PRB_P2,
		PRB_P3,
		PRB_P4,
		PRB_P5,
		PRB_P6,
		PRB_P7,
		PRB_P8,
		PRB_P9,
		PRB_P10,
		PRB_P11,
		PRB_P12,
		PRB_P13,
		PRB_P14,
		PRB_P15,
		PRB_P16,
		PRB_P17,
		PRB_P18,
		PRB_P19,
		PRB_P20,
		PRB_P21,
		PRB_P22,
		PRB_P23,
		PRB_P24,
		PRB_P25
	};

	uint8_t : 3;
};

struct ADCProcessingBlock : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x3D;

	uint8_t ProcessingBlock : 5;
	enum ProcessingBlocks {
		PRB_R1 = 1,
		PRB_R2,
		PRB_R3,
		PRB_R4,
		PRB_R5,
		PRB_R6,
		PRB_R7,
		PRB_R8,
		PRB_R9,
		PRB_R10,
		PRB_R11,
		PRB_R12,
		PRB_R13,
		PRB_R14,
		PRB_R15,
		PRB_R16,
		PRB_R17,
		PRB_R18
	};

	uint8_t : 3;
};

// Register 62 (0x3E): Reserved

struct DACChannelSetup1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x3F;

	uint8_t DACSoftStep : 2;
	enum DACSoftSteps { SoftStep1PerWCLK = 0b00, SoftStep1Per2WCLK = 0b01, SoftStepDisabled = 0b10 };

	uint8_t RightDACDataPath : 2;
	uint8_t LeftDACDataPath : 2;
	enum DACDataPaths { DACDisabled = 0b00, DACNormal = 0b01, DACSwapped = 0b10, DACMono = 0b11 };

	uint8_t RightDACPowerUp : 1;
	uint8_t LeftDACPowerUp : 1;
};

struct DACChannelSetup2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x40;

	uint8_t DACMasterVolumeControl : 2;
	enum DACMasterVolumeControls { Independent = 0b00, LeftFollowsRight = 0b01, RightFollowsLeft = 0b10 };

	uint8_t RightDACMute : 1;
	uint8_t LeftDACMute : 1;

	uint8_t DACAutoMute : 3;
	enum DACAutoMutes {
		AutoMuteDisabled = 0b000,
		AutoMute100 = 0b001,
		AutoMute200,
		AutoMute400,
		AutoMute800,
		AutoMute1600,
		AutoMute3200,
		AutoMute6400
	};

	uint8_t RightModulatorOutputInvert : 1;
};

struct LeftDACVolume : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x41;

	uint8_t Volume;
};

struct RightDACVolume : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x42;

	uint8_t Volume;
};

struct HeadsetDetection : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x43;

	uint8_t ButtonPressDebounce : 2;
	enum ButtonPressDebounces {
		ButtonDebounceDisabled = 0b00,
		ButtonDebounce8ms = 0b01,
		ButtonDebounce16ms = 0b10,
		ButtonDebounce32ms = 0b11
	};

	uint8_t HeadsetDebounce : 3;
	enum HeadsetDebounces {
		Debounce16ms = 0b000,
		Debounce32ms,
		Debounce64ms,
		Debounce128ms,
		Debounce256ms,
		Debounce512ms
	};

	uint8_t HeadsetType : 2;
	enum HeadsetTypes { NotDetected = 0b00, Stereo = 0b01, StereoCellular = 0b11 };

	uint8_t HeadsetDetectionEnabled : 1;
};

struct DRCControl1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x44;

	uint8_t DRCHysteresis : 2;
	enum DRCHysteresiss { Hyst0dB = 0b00, Hyst1dB = 0b01, Hyst2dB = 0b10, Hyst3dB = 0b11 };

	uint8_t DRCThreshold : 3;
	enum DRCThresholds {
		Thresh_3dBFS = 0b000,
		Thresh_6dBFS,
		Thresh_9dBFS,
		Thresh_12dBFS,
		Thresh_15dBFS,
		Thresh_18dBFS,
		Thresh_21dBFS,
		Thresh_24dBFS
	};

	uint8_t RightDRCEnabled : 1;
	uint8_t LeftDRCEnabled : 1;
	uint8_t : 1;
};

struct DRCControl2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x45;

	uint8_t : 3;
	uint8_t DRCHold : 4;
	uint8_t : 1;
};

struct DRCControl3 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x46;

	uint8_t DRCDecayRate : 4;
	uint8_t DRCAttackRate : 4;
};

struct BeepGenerator1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x47;

	uint8_t LeftBeepVolume : 6;
	uint8_t : 1;
	uint8_t BeepEnabled : 1;
};

struct BeepGenerator2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x48;

	uint8_t RightBeepVolume : 6;

	uint8_t BeepMasterVolumeControl : 2;
	enum BeepMasterVolumeControls { BeepIndependent = 0b00, BeepLeftFollowsRight = 0b01, BeepRightFollowsLeft = 0b10 };
};

struct BeepGenerator3 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x49;

	uint8_t BeepSampleLengthMSB;
};

struct BeepGenerator4 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x4A;

	uint8_t BeepSampleLengthMid;
};

struct BeepGenerator5 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x4B;

	uint8_t BeepSampleLengthLSB;
};

struct BeepGenerator6 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x4C;

	uint8_t BeepSinMSB;
};

struct BeepGenerator7 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x4D;

	uint8_t BeepSinLSB;
};

struct BeepGenerator8 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x4E;

	uint8_t BeepCosMSB;
};

struct BeepGenerator9 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x4F;

	uint8_t BeepCosLSB;
};

// Register 80 (0x50): Reserved

struct ADCChannelSetup : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x51;

	uint8_t ADCVolumeSoftStep : 2;
	enum ADCVolumeSoftSteps { SoftStep1PerWCLK = 0b00, SoftStep1Per2WCLK = 0b01, SoftStepDisabled = 0b10 };

	uint8_t RightDigitalMic : 1;
	uint8_t LeftDigitalMic : 1;

	uint8_t DigitalMicInput : 2;
	enum DigitalMicInputs { DigMic_GPIO = 0b00, DigMic_SCLK = 0b01, DigMic_DIN = 0b10 };

	uint8_t RightADCPowerUp : 1;
	uint8_t LeftADCPowerUp : 1;
};

struct ADCFineGainAdjust : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x52;

	uint8_t RightADCFineGain : 3;
	uint8_t RightADCMute : 1;
	uint8_t LeftADCFineGain : 3;
	uint8_t LeftADCMute : 1;
};

struct LeftADCVolume : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x53;

	uint8_t Volume : 7;
	uint8_t : 1;
};

struct RightADCVolume : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x54;

	uint8_t Volume : 7;
	uint8_t : 1;
};

struct ADCPhaseAdjust : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x55;

	uint8_t PhaseCompensation;
};

struct LeftAGCControl1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x56;

	uint8_t GainHysteresis : 2;
	enum GainHysteresiss { GainHystDisabled = 0b00, GainHyst0_5dB = 0b01, GainHyst1_0dB = 0b10, GainHyst1_5dB = 0b11 };

	uint8_t : 2;

	uint8_t TargetLevel : 3;
	enum TargetLevels {
		Target_5_5dBFS = 0b000,
		Target_8_0dBFS,
		Target_10_0dBFS,
		Target_12_0dBFS,
		Target_14_0dBFS,
		Target_17_0dBFS,
		Target_20_0dBFS,
		Target_24_0dBFS
	};

	uint8_t AGCEnabled : 1;
};

struct LeftAGCControl2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x57;

	uint8_t : 1;
	uint8_t NoiseThreshold : 5;

	uint8_t Hysteresis : 2;
	enum Hysteresiss { Hyst1_0dB = 0b00, Hyst2_0dB = 0b01, Hyst4_0dB = 0b10, HystDisabled = 0b11 };
};

struct LeftAGCControl3 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x58;

	uint8_t MaxGain : 7;
	uint8_t : 1;
};

struct LeftAGCControl4 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x59;

	uint8_t AttackTimeScaleFactor : 3;
	uint8_t AttackTime : 5;
};

struct LeftAGCControl5 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x5A;

	uint8_t DecayTimeScaleFactor : 3;
	uint8_t DecayTime : 5;
};

struct LeftAGCControl6 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x5B;

	uint8_t NoiseDebounceTime : 5;
	uint8_t : 3;
};

struct LeftAGCControl7 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x5C;

	uint8_t SignalDebounceTime : 4;
	uint8_t : 4;
};

struct LeftAGCControl8 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x5D;

	uint8_t Gain;
};

struct RightAGCControl1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x5E;

	uint8_t GainHysteresis : 2;
	uint8_t : 2;

	uint8_t TargetLevel : 3;
	uint8_t AGCEnabled : 1;
};

struct RightAGCControl2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x5F;

	uint8_t : 1;
	uint8_t NoiseThreshold : 5;
	uint8_t Hysteresis : 2;
};

struct RightAGCControl3 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x60;

	uint8_t MaxGain : 7;
	uint8_t : 1;
};

struct RightAGCControl4 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x61;

	uint8_t AttackTimeScaleFactor : 3;
	uint8_t AttackTime : 5;
};

struct RightAGCControl5 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x62;

	uint8_t DecayTimeScaleFactor : 3;
	uint8_t DecayTime : 5;
};

struct RightAGCControl6 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x63;

	uint8_t NoiseDebounceTime : 5;
	uint8_t : 3;
};

struct RightAGCControl7 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x64;

	uint8_t SignalDebounceTime : 4;
	uint8_t : 4;
};

struct RightAGCControl8 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x65;

	uint8_t Gain;
};

struct DCMeasurement1 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x66;

	uint8_t DParameter : 5;
	uint8_t FilterType : 1;
	uint8_t RightADCDCMeasurementEnabled : 1;
	uint8_t LeftADCDCMeasurementEnabled : 1;
};

struct DCMeasurement2 : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x67;

	uint8_t IIRAveragingTime : 5;
	uint8_t IIRPeriodicClearing : 1;
	uint8_t ResultUpdateDisabled : 1;
	uint8_t : 1;
};

struct LeftDCMeasurementOutput1 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x68;

	uint8_t Value;
};

struct LeftDCMeasurementOutput2 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x69;

	uint8_t Value;
};

struct LeftDCMeasurementOutput3 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x6A;

	uint8_t Value;
};

struct RightDCMeasurementOutput1 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x6B;

	uint8_t Value;
};

struct RightDCMeasurementOutput2 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x6C;

	uint8_t Value;
};

struct RightDCMeasurementOutput3 : ReadOnly {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x6D;

	uint8_t Value;
};

// Registers 110-127 (0x6E-0x7F): Reserved

// ============================================================
// Page 1 Registers
// ============================================================

struct PowerConfiguration : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x01;

	uint8_t : 3;
	uint8_t DisableWeakAVDDtoDVDD : 1;
	uint8_t : 4;
};

struct LDOControl : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x02;

	uint8_t AVDDLDOPowerUp : 1;
	uint8_t AVDDLDOOverCurrent : 1;
	uint8_t DVDDLDOOverCurrent : 1;
	uint8_t AnalogBlocksDisabled : 1;

	uint8_t AVDDLDOControl : 2;
	enum AVDDLDOControls { AVDD_1_72V = 0b00, AVDD_1_67V = 0b01, AVDD_1_77V = 0b10 };

	uint8_t DVDDLDOControl : 2;
	enum DVDDLDOControls { DVDD_1_72V = 0b00, DVDD_1_67V = 0b01, DVDD_1_77V = 0b10 };
};

struct PlaybackConfiguration1 : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x03;

	uint8_t : 2;

	uint8_t LeftDACPTM : 3;
	enum DACPTMs { PTM_P3_P4 = 0b000, PTM_P2 = 0b001, PTM_P1 = 0b010 };

	uint8_t : 1;

	uint8_t LeftDACDriver : 2;
	enum DACDrivers { ClassAB = 0b00, ClassD = 0b11 };
};

struct PlaybackConfiguration2 : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x04;

	uint8_t : 2;
	uint8_t RightDACPTM : 3;
	uint8_t : 1;
	uint8_t RightDACDriver : 2;
};

// Registers 5-8 (0x05-0x08): Reserved

struct OutputDriverPowerControl : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x09;

	uint8_t MARPowerUp : 1;
	uint8_t MALPowerUp : 1;
	uint8_t LORPowerUp : 1;
	uint8_t LOLPowerUp : 1;
	uint8_t HPRPowerUp : 1;
	uint8_t HPLPowerUp : 1;
	uint8_t : 2;
};

struct CommonModeControl : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x0A;

	uint8_t LDOINRange : 1;
	enum LDOINRanges { Range_1_5V_to_1_95V = 0, Range_1_8V_to_3_6V = 1 };

	uint8_t HPPoweredByLDOIN : 1;
	uint8_t : 1;
	uint8_t LOCommonMode : 1;

	uint8_t HPCommonMode : 2;
	enum HPCommonModes { HP_FullChip = 0b00, HP_1_25V = 0b01, HP_1_5V = 0b10, HP_1_65V = 0b11 };

	uint8_t FullChipCommonMode : 1;
	enum FullChipCommonModes { CM_0_9V = 0, CM_0_75V = 1 };

	uint8_t : 1;
};

struct OverCurrentProtection : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x0B;

	uint8_t PowerDownOnOverCurrent : 1;

	uint8_t OverCurrentDebounce : 3;
	enum OverCurrentDebounces {
		Debounce_None = 0b000,
		Debounce_8ms = 0b001,
		Debounce_16ms = 0b010,
		Debounce_32ms = 0b011,
		Debounce_64ms = 0b100,
		Debounce_128ms = 0b101,
		Debounce_256ms = 0b110,
		Debounce_512ms = 0b111,
	};

	uint8_t OverCurrentDetectionEnabled : 1;
	uint8_t : 3;
};

struct HPLRoutingSelection : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x0C;

	uint8_t MARtoHPL : 1;
	uint8_t MALtoHPL : 1;
	uint8_t IN1LtoHPL : 1;
	uint8_t LeftDACtoHPL : 1;
	uint8_t : 4;
};

struct HPRRoutingSelection : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x0D;

	uint8_t HPLtoHPR : 1;
	uint8_t MARtoHPR : 1;
	uint8_t IN1RtoHPR : 1;
	uint8_t RightDACtoHPR : 1;
	uint8_t LeftDACNegtoHPR : 1;
	uint8_t : 3;
};

struct LOLRoutingSelection : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x0E;

	uint8_t LORtoLOL : 1;
	uint8_t MALtoLOL : 1;
	uint8_t : 1;
	uint8_t LeftDACtoLOL : 1;
	uint8_t RightDACNegtoLOL : 1;
	uint8_t : 3;
};

struct LORRoutingSelection : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x0F;

	uint8_t : 1;
	uint8_t MARtoLOR : 1;
	uint8_t : 1;
	uint8_t RightDACtoLOR : 1;
	uint8_t : 4;
};

struct HPLDriverGain : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x10;

	uint8_t Gain : 6;
	uint8_t Mute : 1;
	uint8_t : 1;
};

struct HPRDriverGain : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x11;

	uint8_t Gain : 6;
	uint8_t Mute : 1;
	uint8_t : 1;
};

struct LOLDriverGain : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x12;

	uint8_t Gain : 6;
	uint8_t Mute : 1;
	uint8_t : 1;
};

struct LORDriverGain : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x13;

	uint8_t Gain : 6;
	uint8_t Mute : 1;
	uint8_t : 1;
};

struct HeadphoneDriverStartup : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x14;

	uint8_t RampResistance : 2;
	enum RampResistances { Ramp_25k = 0b00, Ramp_6k = 0b01, Ramp_2k = 0b10 };

	uint8_t PowerUpTimeConstants : 4;

	uint8_t SoftRoutingStepTime : 2;
	enum SoftRoutingStepTimes { Step_0ms = 0b00, Step_50ms = 0b01, Step_100ms = 0b10, Step_200ms = 0b11 };
};

// Register 21 (0x15): Reserved

struct IN1LtoHPLVolume : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x16;

	uint8_t Volume : 7;
	uint8_t : 1;
};

struct IN1RtoHPRVolume : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x17;

	uint8_t Volume : 7;
	uint8_t : 1;
};

struct MixerAmpLeftVolume : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x18;

	uint8_t Volume : 6;
	uint8_t : 2;
};

struct MixerAmpRightVolume : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x19;

	uint8_t Volume : 6;
	uint8_t : 2;
};

// Registers 26-50 (0x1A-0x32): Reserved

struct MICBIASConfiguration : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x33;

	uint8_t : 3;
	uint8_t MICBIASFromLDOIN : 1;

	uint8_t MICBIASVoltage : 2;
	enum MICBIASVoltages {
		MICBIAS_1_04V_or_1_25V = 0b00,
		MICBIAS_1_425V_or_1_7V = 0b01,
		MICBIAS_2_075V_or_2_5V = 0b10,
		MICBIAS_PowerSupply = 0b11,
	};

	uint8_t MICBIASPowerUp : 1;
	uint8_t : 1;
};

struct LeftMICPGAPositiveRouting : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x34;

	uint8_t IN1RtoLeftMICPGA : 2;
	uint8_t IN3LtoLeftMICPGA : 2;
	uint8_t IN2LtoLeftMICPGA : 2;
	uint8_t IN1LtoLeftMICPGA : 2;
	enum InputRoutings { NotRouted = 0b00, Routed_10k = 0b01, Routed_20k = 0b10, Routed_40k = 0b11 };
};

// Register 53 (0x35): Reserved

struct LeftMICPGANegativeRouting : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x36;

	uint8_t CM2LtoLeftMICPGA : 2;
	uint8_t IN3RtoLeftMICPGA : 2;
	uint8_t IN2RtoLeftMICPGA : 2;
	uint8_t CM1LtoLeftMICPGA : 2;
};

struct RightMICPGAPositiveRouting : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x37;

	uint8_t IN2LtoRightMICPGA : 2;
	uint8_t IN3RtoRightMICPGA : 2;
	uint8_t IN2RtoRightMICPGA : 2;
	uint8_t IN1RtoRightMICPGA : 2;
};

// Register 56 (0x38): Reserved

struct RightMICPGANegativeRouting : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x39;

	uint8_t CM2RtoRightMICPGA : 2;
	uint8_t IN3LtoRightMICPGA : 2;
	uint8_t IN1LtoRightMICPGA : 2;
	uint8_t CM1RtoRightMICPGA : 2;
};

struct FloatingInputConfiguration : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x3A;

	uint8_t : 2;
	uint8_t IN3RWeaklyDriven : 1;
	uint8_t IN3LWeaklyDriven : 1;
	uint8_t IN2RWeaklyDriven : 1;
	uint8_t IN2LWeaklyDriven : 1;
	uint8_t IN1RWeaklyDriven : 1;
	uint8_t IN1LWeaklyDriven : 1;
};

struct LeftMICPGAVolume : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x3B;

	uint8_t Volume : 7;
	uint8_t GainDisabled : 1;
};

struct RightMICPGAVolume : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x3C;

	uint8_t Volume : 7;
	uint8_t GainDisabled : 1;
};

struct ADCPowerTuneConfiguration : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x3D;

	uint8_t PTMConfiguration;
	enum PTMConfigurations { PTM_R4 = 0x00, PTM_R3 = 0x64, PTM_R2 = 0xB6, PTM_R1 = 0xFF };
};

struct ADCAnalogVolumeFlag : ReadOnly {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x3E;

	uint8_t RightVolumeApplied : 1;
	uint8_t LeftVolumeApplied : 1;
	uint8_t : 6;
};

struct DACAnalogGainFlag : ReadOnly {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x3F;

	uint8_t MARVolumeApplied : 1;
	uint8_t MALVolumeApplied : 1;
	uint8_t IN1RtoHPRVolumeApplied : 1;
	uint8_t IN1LtoHPLVolumeApplied : 1;
	uint8_t LORGainApplied : 1;
	uint8_t LOLGainApplied : 1;
	uint8_t HPRGainApplied : 1;
	uint8_t HPLGainApplied : 1;
};

// Registers 64-70 (0x40-0x46): Reserved

struct AnalogInputQuickCharging : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x47;

	uint8_t PowerUpTime : 6;
	enum PowerUpTimes { Default = 0b000000, Time_3_1ms = 0b110001, Time_6_4ms = 0b110010, Time_1_6ms = 0b110011 };

	uint8_t : 2;
};

// Registers 72-122 (0x48-0x7A): Reserved

struct ReferencePowerUpConfiguration : ReadWrite {
	static constexpr uint8_t Page = 0x01;
	static constexpr uint8_t Address = 0x7B;

	uint8_t Configuration : 3;
	enum Configurations {
		AutoSlow = 0b000,
		Auto40ms = 0b001,
		Auto80ms = 0b010,
		Auto120ms = 0b011,
		ForceSlow = 0b100,
		Force40ms = 0b101,
		Force80ms = 0b110,
		Force120ms = 0b111,
	};

	uint8_t : 5;
};

// Registers 124-127 (0x7C-0x7F): Reserved

using Registers = std::variant<PageSelect,
							   SwReset,
							   ClockSettingMultiplexors,
							   ClockSettingPLLPandRValues,
							   ClockSettingPLLJValues,
							   ClockSettingPLLDValuesMSB,
							   ClockSettingPLLDValuesLSB,
							   ClockSettingNDACValues,
							   ClockSettingMDACValues,
							   DACOSRSettingMSB,
							   DACOSRSettingLSB,
							   ClockSettingNADCValues,
							   ClockSettingMADCValues,
							   ADCOversampling,
							   ClockSettingCDIVCLKIN,
							   ClockSettingCLKOUTMDiv,
							   AudioInterfaceSetting1,
							   AudioInterfaceSetting2,
							   AudioInterfaceSetting3,
							   ClockSettingBCLKNDiv,
							   AudioInterfaceSetting4,
							   AudioInterfaceSetting5,
							   AudioInterfaceSetting6,
							   DigitalInterfaceMiscSetting,
							   ADCFlagRegister,
							   DACFlagRegister1,
							   DACFlagRegister2,
							   StickyFlagRegister1,
							   InterruptFlagRegister1,
							   StickyFlagRegister2,
							   StickyFlagRegister3,
							   InterruptFlagRegister2,
							   InterruptFlagRegister3,
							   INT1InterruptControl,
							   INT2InterruptControl,
							   GPIOControl,
							   DOUTControl,
							   DINControl,
							   MISOControl,
							   SCLKControl,
							   DACProcessingBlock,
							   ADCProcessingBlock,
							   DACChannelSetup1,
							   DACChannelSetup2,
							   LeftDACVolume,
							   RightDACVolume,
							   HeadsetDetection,
							   DRCControl1,
							   DRCControl2,
							   DRCControl3,
							   BeepGenerator1,
							   BeepGenerator2,
							   BeepGenerator3,
							   BeepGenerator4,
							   BeepGenerator5,
							   BeepGenerator6,
							   BeepGenerator7,
							   BeepGenerator8,
							   BeepGenerator9,
							   ADCChannelSetup,
							   ADCFineGainAdjust,
							   LeftADCVolume,
							   RightADCVolume,
							   ADCPhaseAdjust,
							   LeftAGCControl1,
							   LeftAGCControl2,
							   LeftAGCControl3,
							   LeftAGCControl4,
							   LeftAGCControl5,
							   LeftAGCControl6,
							   LeftAGCControl7,
							   LeftAGCControl8,
							   RightAGCControl1,
							   RightAGCControl2,
							   RightAGCControl3,
							   RightAGCControl4,
							   RightAGCControl5,
							   RightAGCControl6,
							   RightAGCControl7,
							   RightAGCControl8,
							   DCMeasurement1,
							   DCMeasurement2,
							   LeftDCMeasurementOutput1,
							   LeftDCMeasurementOutput2,
							   LeftDCMeasurementOutput3,
							   RightDCMeasurementOutput1,
							   RightDCMeasurementOutput2,
							   RightDCMeasurementOutput3,
							   PowerConfiguration,
							   LDOControl,
							   PlaybackConfiguration1,
							   PlaybackConfiguration2,
							   OutputDriverPowerControl,
							   CommonModeControl,
							   OverCurrentProtection,
							   HPLRoutingSelection,
							   HPRRoutingSelection,
							   LOLRoutingSelection,
							   LORRoutingSelection,
							   HPLDriverGain,
							   HPRDriverGain,
							   LOLDriverGain,
							   LORDriverGain,
							   HeadphoneDriverStartup,
							   IN1LtoHPLVolume,
							   IN1RtoHPRVolume,
							   MixerAmpLeftVolume,
							   MixerAmpRightVolume,
							   MICBIASConfiguration,
							   LeftMICPGAPositiveRouting,
							   LeftMICPGANegativeRouting,
							   RightMICPGAPositiveRouting,
							   RightMICPGANegativeRouting,
							   FloatingInputConfiguration,
							   LeftMICPGAVolume,
							   RightMICPGAVolume,
							   ADCPowerTuneConfiguration,
							   ADCAnalogVolumeFlag,
							   DACAnalogGainFlag,
							   AnalogInputQuickCharging,
							   ReferencePowerUpConfiguration>;

} // namespace mdrivlib::CodecTLV320AIC3204Register
