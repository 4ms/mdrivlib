#pragma once
#include "drivers/bus_register.hh"
#include <cstdint>

namespace mdrivlib::TLA2528
{
using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

struct SystemStatus : ReadWrite {
	enum : uint8_t { Address = 0x00 };

	uint8_t : 7;
	uint8_t RsvdAlwaysHigh : 1;

	// constexpr operator uint8_t() {
	// 	return (RevID << 0) | (ProductID << 2) | (VersionID << 4);
	// }
	constexpr static SystemStatus make(uint8_t raw) {
		return {
			.RsvdAlwaysHigh = Bits<7>(raw),
		};
	}
};

struct GeneralCfg : ReadWrite {
	enum : uint8_t { Address = 0x01 };

	uint8_t Reset : 1; //W only
	uint8_t Calibrate : 1;
	uint8_t ForceChannelsAnalog : 1;
	uint8_t StartConversion : 1; //W only
	uint8_t : 4;

	// constexpr operator uint8_t() {
	// 	return (PullDownCC1 << 0) | (PullDownCC2 << 1) | (MeasureCC1 << 2) | (MeasureCC2 << 3) |
	// 		   (ConnectVConnCC1 << 4) | (ConnectVConnCC2 << 5) | (PullUpCC1 << 6) | (PullUpCC2 << 7);
	// }

	// constexpr static Switches0 make(uint8_t raw) {
	// 	return {.PullDownCC1 = Bits<0>(raw),
	// 			.PullDownCC2 = Bits<1>(raw),
	// 			.MeasureCC1 = Bits<2>(raw),
	// 			.MeasureCC2 = Bits<3>(raw),
	// 			.ConnectVConnCC1 = Bits<4>(raw),
	// 			.ConnectVConnCC2 = Bits<5>(raw),
	// 			.PullUpCC1 = Bits<6>(raw),
	// 			.PullUpCC2 = Bits<7>(raw)};
	// }
};

struct DataCfg : ReadWrite {
	enum : uint8_t { Address = 0x02 };

	uint8_t : 4;
	uint8_t AppendStatus : 2;
	uint8_t : 1;
	uint8_t DebugOutput0xA5A : 1;

	// constexpr operator uint8_t() {
	// 	return (EnableTXCC1 << 0) | (EnableTXCC2 << 1) | (AutoCRC << 2) | (DataRoleSrc << 4) | (SpecRev << 5) |
	// 		   (PowerRoleSrc << 7);
	// }
	// constexpr static Switches1 make(uint8_t raw) {
	// 	return {.EnableTXCC1 = Bits<0>(raw),
	// 			.EnableTXCC2 = Bits<1>(raw),
	// 			.AutoCRC = Bits<2>(raw),
	// 			.DataRoleSrc = Bits<4>(raw),
	// 			.SpecRev = Bits<5, 6>(raw),
	// 			.PowerRoleSrc = Bits<7>(raw)};
	// }
};

struct OversampleCfg : ReadWrite {
	enum : uint8_t { Address = 0x03 };

	uint8_t OversampleBits : 3;
	uint8_t : 5;

	// constexpr operator uint8_t() {
	// 	return (DAC << 0) | (MeasureVBUS << 6);
	// }
	// constexpr static Measure make(uint8_t raw) {
	// 	return {
	// 		.DAC = Bits<0, 1, 2, 3, 4, 5>(raw),
	// 		.MeasureVBUS = Bits<6>(raw),
	// 	};
	// }
};

struct OpMode : ReadWrite {
	enum : uint8_t { Address = 0x04 };

	uint8_t ClockDiv : 4;
	uint8_t OscSel : 1;
	uint8_t : 3;

	enum OscSels { OscSelLowPower = 1, OscSelHighSpeed = 0 };

	// constexpr operator uint8_t() {
	// 	return (BMCSlicerDAC << 0) | (SliderDACHys << 6);
	// }
	// constexpr static Slice make(uint8_t raw) {
	// 	return {
	// 		.BMCSlicerDAC = Bits<0, 1, 2, 3, 4, 5>(raw),
	// 		.SliderDACHys = Bits<6, 7>(raw),
	// 	};
	// }
};

struct PinCfg : ReadWrite {
	enum : uint8_t { Address = 0x05 };

	uint8_t BitMaskDigitalGPIO; //bitmask 1 = GPIO, 0 = analog

	// constexpr operator uint8_t() {
	// 	return (AutoStartOnCRCRx << 1) | (HostCurrent << 2) | (MaskAllInt << 5);
	// }
	// constexpr static Control0 make(uint8_t raw) {
	// 	return {
	// 		.AutoStartOnCRCRx = Bits<1>(raw),
	// 		.HostCurrent = Bits<2, 3>(raw),
	// 		.MaskAllInt = Bits<5>(raw),
	// 	};
	// }
};

struct GPIOCfg : WriteClear {
	enum : uint8_t { Address = 0x07 };

	uint8_t BitMaskOutputs; //bitmask 1 = output, 0 = input

	// constexpr static Control0WC make(uint8_t raw) {
	// 	return {
	// 		.TXStart = Bits<0>(raw),
	// 		.TXFlush = Bits<6>(raw),
	// 	};
	// }
};

struct GPOCgf : ReadWrite {
	enum : uint8_t { Address = 0x09 };

	uint8_t BitMaskOutputsPushPull; //bitmask: 1 = push-pull, 0 = open-drain

	// constexpr operator uint8_t() {
	// 	return (EnableSOP1 << 0) | (EnableSOP2 << 1) | (RXFlush << 2) | (BISTMode2 << 4) | (EnableSOP1Dbg << 5) |
	// 		   (EnableSOP2Dbg << 6);
	// }
	// constexpr static Control1 make(uint8_t raw) {
	// 	return {.EnableSOP1 = Bits<0>(raw),
	// 			.EnableSOP2 = Bits<1>(raw),
	// 			.BISTMode2 = Bits<4>(raw),
	// 			.EnableSOP1Dbg = Bits<5>(raw),
	// 			.EnableSOP2Dbg = Bits<6>(raw)};
	// }
};

struct GPOValue : ReadWrite {
	enum : uint8_t { Address = 0x0b };

	uint8_t BitMaskSetOutput;

	// constexpr operator uint8_t() {
	// 	return (Toggle << 0) | (PollingMode << 1) | (WakeEnable << 3) | (ToggleIgnoreRa << 5) | (ToggleTime << 6);
	// }
	// constexpr static Control2 make(uint8_t raw) {
	// 	return {.Toggle = Bits<0>(raw),
	// 			.PollingMode = Bits<1, 2>(raw),
	// 			.WakeEnable = Bits<3>(raw),
	// 			.ToggleIgnoreRa = Bits<5>(raw),
	// 			.ToggleTime = Bits<6, 7>(raw)};
	// }
};

struct GPIValue : ReadOnly {
	enum : uint8_t { Address = 0x0d };

	uint8_t BitMaskReadInput;

	// constexpr operator uint8_t() {
	// 	return (AutoRetryCRC << 0) | (NumRetries << 1) | (AutoSoftReset << 3) | (AutoHardReset << 4) |
	// 		   (BISTTMode << 5) | (SendHardReset << 6);
	// }
	// constexpr static Control3 make(uint8_t raw) {
	// 	return {.AutoRetryCRC = Bits<0>(raw),
	// 			.NumRetries = Bits<1, 2>(raw),
	// 			.AutoSoftReset = Bits<3>(raw),
	// 			.AutoHardReset = Bits<4>(raw),
	// 			.BISTTMode = Bits<5>(raw)};
	// }
};

struct SequenceCfg : ReadWrite {
	enum : uint8_t { Address = 0x10 };

	uint8_t SeqModeAuto : 1;
	uint8_t SeqModeReserved : 1;
	uint8_t : 2;
	uint8_t SeqStart : 1;
	uint8_t : 3;

	// constexpr operator uint8_t() {
	// 	return (HostCurrentReq << 0) | (Collision << 1) | (Wake << 2) | (Alert << 3) | (CRCCheck << 4) |
	// 		   (CompChange << 5) | (CCBusActivity << 6) | (VBusOK << 7);
	// }
	// constexpr static Mask make(uint8_t raw) {
	// 	return {.HostCurrentReq = Bits<0>(raw),
	// 			.Collision = Bits<1>(raw),
	// 			.Wake = Bits<2>(raw),
	// 			.Alert = Bits<3>(raw),
	// 			.CRCCheck = Bits<4>(raw),
	// 			.CompChange = Bits<5>(raw),
	// 			.CCBusActivity = Bits<6>(raw),
	// 			.VBusOK = Bits<7>(raw)};
	// }
};

struct ChannelSel : ReadWrite {
	enum : uint8_t { Address = 0x11 };

	uint8_t ManualChannelSel : 3;
	uint8_t : 1;
	uint8_t : 4;

	// constexpr operator uint8_t() {
	// 	return (BandGapAndWake << 0) | (MeasureBlock << 1) | (RXAndCurrentRefs << 2) | (IntOsc << 3);
	// }
	// constexpr static Power make(uint8_t raw) {
	// 	return {
	// 		.BandGapAndWake = Bits<0>(raw),
	// 		.MeasureBlock = Bits<1>(raw),
	// 		.RXAndCurrentRefs = Bits<2>(raw),
	// 		.IntOsc = Bits<3>(raw),
	// 	};
	// }
};

struct AutoSeqChanSel : WriteClear {
	enum : uint8_t { Address = 0x12 };

	uint8_t BitMaskChannelEnabled;

	// constexpr operator uint8_t() {
	// 	return (SWReset << 0) | (PDReset << 1);
	// }
};

} // namespace mdrivlib::TLA2528

// How this was created:
// First, manually created the bitfield structs.
//
// Create operator uint8_t() for bitfields with all 1-bit fields:
//%s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << 1) | (\3 << 2) | (\4 << 3) | (\5 << 4) | (\6 << 5) | (\7 << 6) | (\8 << 7); }/
// Create operator uint8_t for bitfields with seven fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X) | (\5 << X) | (\6 << X) | (\7 << X); }/
// Create operator uint8_t for bitfields with six fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X) | (\5 << X) | (\6 << X); }/
// Create operator uint8_t for bitfields with five fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X) | (\5 << X); }/
// Create operator uint8_t for bitfields with four fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X); }/
// Three, two, and one fields are easy to do manually (typically have one or more _res)
// Afterwards, remove all (<< \d);
// %s/ | (<< \d)//g
