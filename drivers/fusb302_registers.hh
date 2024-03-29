#pragma once
#include "drivers/bus_register.hh"
#include <cstdint>

namespace FUSB302
{
using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

struct ID : ReadOnly {
	enum : uint8_t { Address = 0x01 };

	uint8_t RevID : 2;
	uint8_t ProductID : 2;
	uint8_t VersionID : 4;

	constexpr operator uint8_t() {
		return (RevID << 0) | (ProductID << 2) | (VersionID << 4);
	}
	constexpr static ID make(uint8_t raw) {
		return {
			.RevID = Bits<0, 1>(raw),
			.ProductID = Bits<2, 3>(raw),
			.VersionID = Bits<4, 5, 6, 7>(raw),
		};
	}
};

struct Switches0 : ReadWrite {
	enum : uint8_t { Address = 0x02 };

	uint8_t PullDownCC1 : 1;
	uint8_t PullDownCC2 : 1;
	uint8_t MeasureCC1 : 1;
	uint8_t MeasureCC2 : 1;
	uint8_t ConnectVConnCC1 : 1;
	uint8_t ConnectVConnCC2 : 1;
	uint8_t PullUpCC1 : 1;
	uint8_t PullUpCC2 : 1;

	constexpr operator uint8_t() {
		return (PullDownCC1 << 0) | (PullDownCC2 << 1) | (MeasureCC1 << 2) | (MeasureCC2 << 3) |
			   (ConnectVConnCC1 << 4) | (ConnectVConnCC2 << 5) | (PullUpCC1 << 6) | (PullUpCC2 << 7);
	}

	constexpr static Switches0 make(uint8_t raw) {
		return {.PullDownCC1 = Bits<0>(raw),
				.PullDownCC2 = Bits<1>(raw),
				.MeasureCC1 = Bits<2>(raw),
				.MeasureCC2 = Bits<3>(raw),
				.ConnectVConnCC1 = Bits<4>(raw),
				.ConnectVConnCC2 = Bits<5>(raw),
				.PullUpCC1 = Bits<6>(raw),
				.PullUpCC2 = Bits<7>(raw)};
	}
};

struct Switches1 : ReadWrite {
	enum : uint8_t { Address = 0x03 };

	uint8_t EnableTXCC1 : 1;
	uint8_t EnableTXCC2 : 1;
	uint8_t AutoCRC : 1;
	uint8_t : 1;
	uint8_t DataRoleSrc : 1;
	uint8_t SpecRev : 2;
	uint8_t PowerRoleSrc : 1;

	constexpr operator uint8_t() {
		return (EnableTXCC1 << 0) | (EnableTXCC2 << 1) | (AutoCRC << 2) | (DataRoleSrc << 4) | (SpecRev << 5) |
			   (PowerRoleSrc << 7);
	}
	constexpr static Switches1 make(uint8_t raw) {
		return {.EnableTXCC1 = Bits<0>(raw),
				.EnableTXCC2 = Bits<1>(raw),
				.AutoCRC = Bits<2>(raw),
				.DataRoleSrc = Bits<4>(raw),
				.SpecRev = Bits<5, 6>(raw),
				.PowerRoleSrc = Bits<7>(raw)};
	}
};

struct Measure : ReadWrite {
	enum : uint8_t { Address = 0x04 };

	uint8_t DAC : 6;
	uint8_t MeasureVBUS : 1;
	uint8_t : 1;

	constexpr operator uint8_t() {
		return (DAC << 0) | (MeasureVBUS << 6);
	}
	constexpr static Measure make(uint8_t raw) {
		return {
			.DAC = Bits<0, 1, 2, 3, 4, 5>(raw),
			.MeasureVBUS = Bits<6>(raw),
		};
	}
};

struct Slice : ReadWrite {
	enum : uint8_t { Address = 0x05 };

	uint8_t BMCSlicerDAC : 6;
	uint8_t SliderDACHys : 2;

	constexpr operator uint8_t() {
		return (BMCSlicerDAC << 0) | (SliderDACHys << 6);
	}
	constexpr static Slice make(uint8_t raw) {
		return {
			.BMCSlicerDAC = Bits<0, 1, 2, 3, 4, 5>(raw),
			.SliderDACHys = Bits<6, 7>(raw),
		};
	}
};

struct Control0 : ReadWrite {
	enum : uint8_t { Address = 0x06 };

	uint8_t /*TXStart*/ : 1;
	uint8_t AutoStartOnCRCRx : 1;
	uint8_t HostCurrent : 2;
	uint8_t : 1;
	uint8_t MaskAllInt : 1;
	uint8_t /*TXFlush*/ : 1;
	uint8_t : 1;

	enum HostCurrents {
		NoCurrent = 0b00,
		DefaultCurrent = 0b01,
		MediumCurrent = 0b10,
		HighCurrent = 0b11,
	};

	constexpr operator uint8_t() {
		return (AutoStartOnCRCRx << 1) | (HostCurrent << 2) | (MaskAllInt << 5);
	}
	constexpr static Control0 make(uint8_t raw) {
		return {
			.AutoStartOnCRCRx = Bits<1>(raw),
			.HostCurrent = Bits<2, 3>(raw),
			.MaskAllInt = Bits<5>(raw),
		};
	}
};

struct Control0WC : WriteClear {
	enum : uint8_t { Address = 0x06 };

	uint8_t TXStart : 1;
	uint8_t : 1;
	uint8_t : 2;
	uint8_t : 1;
	uint8_t : 1;
	uint8_t TXFlush : 1;
	uint8_t : 1;

	constexpr static Control0WC make(uint8_t raw) {
		return {
			.TXStart = Bits<0>(raw),
			.TXFlush = Bits<6>(raw),
		};
	}
};

// TODO: separate into a RW and a WC struct?
struct Control1 : ReadWrite, WriteClear {
	enum : uint8_t { Address = 0x07 };

	uint8_t EnableSOP1 : 1;
	uint8_t EnableSOP2 : 1;
	uint8_t RXFlush : 1 = 0; //W/C
	uint8_t : 1;
	uint8_t BISTMode2 : 1;
	uint8_t EnableSOP1Dbg : 1;
	uint8_t EnableSOP2Dbg : 1;
	uint8_t : 1;

	constexpr operator uint8_t() {
		return (EnableSOP1 << 0) | (EnableSOP2 << 1) | (RXFlush << 2) | (BISTMode2 << 4) | (EnableSOP1Dbg << 5) |
			   (EnableSOP2Dbg << 6);
	}
	constexpr static Control1 make(uint8_t raw) {
		return {.EnableSOP1 = Bits<0>(raw),
				.EnableSOP2 = Bits<1>(raw),
				.BISTMode2 = Bits<4>(raw),
				.EnableSOP1Dbg = Bits<5>(raw),
				.EnableSOP2Dbg = Bits<6>(raw)};
	}
};

struct Control2 : ReadWrite {
	enum : uint8_t { Address = 0x08 };

	uint8_t Toggle : 1;
	uint8_t PollingMode : 2;
	uint8_t WakeEnable : 1;
	uint8_t : 1;
	uint8_t ToggleIgnoreRa : 1;
	uint8_t ToggleTime : 2;

	constexpr operator uint8_t() {
		return (Toggle << 0) | (PollingMode << 1) | (WakeEnable << 3) | (ToggleIgnoreRa << 5) | (ToggleTime << 6);
	}
	constexpr static Control2 make(uint8_t raw) {
		return {.Toggle = Bits<0>(raw),
				.PollingMode = Bits<1, 2>(raw),
				.WakeEnable = Bits<3>(raw),
				.ToggleIgnoreRa = Bits<5>(raw),
				.ToggleTime = Bits<6, 7>(raw)};
	}
	enum PollingModes {
		PollDRP = 0b01,
		PollSNK = 0b10,
		PollSRC = 0b11,
	};
};

// TODO: separate into a RW and a WC struct?
//Control3<ReadWrite> and Control3<WriteClear>?
//or Control3_RW and Control3_WC?
struct Control3 : ReadWrite, WriteClear {
	enum : uint8_t { Address = 0x09 };

	uint8_t AutoRetryCRC : 1;
	uint8_t NumRetries : 2;
	uint8_t AutoSoftReset : 1;
	uint8_t AutoHardReset : 1;
	uint8_t BISTTMode : 1;
	uint8_t SendHardReset : 1 {}; //W/C
	uint8_t : 1;

	constexpr operator uint8_t() {
		return (AutoRetryCRC << 0) | (NumRetries << 1) | (AutoSoftReset << 3) | (AutoHardReset << 4) |
			   (BISTTMode << 5) | (SendHardReset << 6);
	}
	constexpr static Control3 make(uint8_t raw) {
		return {.AutoRetryCRC = Bits<0>(raw),
				.NumRetries = Bits<1, 2>(raw),
				.AutoSoftReset = Bits<3>(raw),
				.AutoHardReset = Bits<4>(raw),
				.BISTTMode = Bits<5>(raw)};
	}
};

struct Mask : ReadWrite {
	enum : uint8_t { Address = 0x0A };

	uint8_t HostCurrentReq : 1;
	uint8_t Collision : 1;
	uint8_t Wake : 1;
	uint8_t Alert : 1;
	uint8_t CRCCheck : 1;
	uint8_t CompChange : 1;
	uint8_t CCBusActivity : 1;
	uint8_t VBusOK : 1;

	constexpr operator uint8_t() {
		return (HostCurrentReq << 0) | (Collision << 1) | (Wake << 2) | (Alert << 3) | (CRCCheck << 4) |
			   (CompChange << 5) | (CCBusActivity << 6) | (VBusOK << 7);
	}
	constexpr static Mask make(uint8_t raw) {
		return {.HostCurrentReq = Bits<0>(raw),
				.Collision = Bits<1>(raw),
				.Wake = Bits<2>(raw),
				.Alert = Bits<3>(raw),
				.CRCCheck = Bits<4>(raw),
				.CompChange = Bits<5>(raw),
				.CCBusActivity = Bits<6>(raw),
				.VBusOK = Bits<7>(raw)};
	}
};

struct Power : ReadWrite {
	enum : uint8_t { Address = 0x0B };

	uint8_t BandGapAndWake : 1;
	uint8_t MeasureBlock : 1;
	uint8_t RXAndCurrentRefs : 1;
	uint8_t IntOsc : 1;
	uint8_t : 4;

	constexpr operator uint8_t() {
		return (BandGapAndWake << 0) | (MeasureBlock << 1) | (RXAndCurrentRefs << 2) | (IntOsc << 3);
	}
	constexpr static Power make(uint8_t raw) {
		return {
			.BandGapAndWake = Bits<0>(raw),
			.MeasureBlock = Bits<1>(raw),
			.RXAndCurrentRefs = Bits<2>(raw),
			.IntOsc = Bits<3>(raw),
		};
	}
};

struct Reset : WriteClear {
	enum : uint8_t { Address = 0x0C };

	uint8_t SWReset : 1;
	uint8_t PDReset : 1;
	uint8_t : 6;

	constexpr operator uint8_t() {
		return (SWReset << 0) | (PDReset << 1);
	}
};

struct OCP : ReadWrite {
	enum : uint8_t { Address = 0x0D };

	uint8_t OverCurrentDivEighths : 3;
	uint8_t OverCurrentMax : 1;
	uint8_t : 4;

	constexpr operator uint8_t() {
		return (OverCurrentDivEighths << 0) | (OverCurrentMax << 3);
	}
	constexpr static OCP make(uint8_t raw) {
		return {
			.OverCurrentDivEighths = Bits<0, 1, 2>(raw),
			.OverCurrentMax = Bits<3>(raw),
		};
	}

	enum OverCurrentMax {
		_80mA = 0,
		_800mA = 1,
	};
};

struct MaskA : ReadWrite {
	enum : uint8_t { Address = 0x0E };

	uint8_t HardResetRx : 1;
	uint8_t SoftResetRx : 1;
	uint8_t TxSent : 1;
	uint8_t HardResetSent : 1;
	uint8_t RetryFail : 1;
	uint8_t SoftFail : 1;
	uint8_t ToggleDone : 1;
	uint8_t OCPTempEvent : 1;

	constexpr operator uint8_t() {
		return (HardResetRx << 0) | (SoftResetRx << 1) | (TxSent << 2) | (HardResetSent << 3) | (RetryFail << 4) |
			   (SoftFail << 5) | (ToggleDone << 6) | (OCPTempEvent << 7);
	}
	constexpr static MaskA make(uint8_t raw) {
		return {.HardResetRx = Bits<0>(raw),
				.SoftResetRx = Bits<1>(raw),
				.TxSent = Bits<2>(raw),
				.HardResetSent = Bits<3>(raw),
				.RetryFail = Bits<4>(raw),
				.SoftFail = Bits<5>(raw),
				.ToggleDone = Bits<6>(raw),
				.OCPTempEvent = Bits<7>(raw)};
	}
};

struct MaskB : ReadWrite {
	enum : uint8_t { Address = 0x0F };

	uint8_t GoodCRCSent : 1;
	uint8_t : 7;

	constexpr operator uint8_t() {
		return (GoodCRCSent << 0);
	}
	constexpr static MaskB make(uint8_t raw) {
		return {.GoodCRCSent = Bits<0>(raw)};
	}
};

struct Control4 : ReadWrite {
	enum : uint8_t { Address = 0x10 };

	uint8_t ToggleExitAudio : 1;
	uint8_t : 7;

	constexpr operator uint8_t() {
		return (ToggleExitAudio << 0);
	}
	constexpr static Control4 make(uint8_t raw) {
		return {.ToggleExitAudio = Bits<0>(raw)};
	}
};

struct Status0A : ReadOnly {
	enum : uint8_t { Address = 0x3C };

	uint8_t HardReset : 1;
	uint8_t SoftReset : 1;
	uint8_t Power2 : 1;
	uint8_t Power3 : 1;
	uint8_t RetryFail : 1;
	uint8_t SoftFail : 1;
	uint8_t : 2;

	constexpr operator uint8_t() {
		return (HardReset << 0) | (SoftReset << 1) | (Power2 << 2) | (Power3 << 3) | (RetryFail << 4) | (SoftFail << 5);
	}
	constexpr static Status0A make(uint8_t raw) {
		return {
			.HardReset = Bits<0>(raw),
			.SoftReset = Bits<1>(raw),
			.Power2 = Bits<2>(raw),
			.Power3 = Bits<3>(raw),
			.RetryFail = Bits<4>(raw),
			.SoftFail = Bits<5>(raw),
		};
	}
};

struct Status1A : ReadOnly {
	enum : uint8_t { Address = 0x3D };

	uint8_t RXSOP : 1;
	uint8_t RXSOP1Dbg : 1;
	uint8_t RXSOP2Dbg : 1;
	uint8_t ToggleOutcomeIsCC1 : 1;
	uint8_t ToggleOutcomeIsCC2 : 1;
	uint8_t ToggleOutcomeIsSink : 1;
	uint8_t : 2;

	constexpr operator uint8_t() {
		return (RXSOP << 0) | (RXSOP1Dbg << 1) | (RXSOP2Dbg << 2) | (ToggleOutcomeIsCC1 << 3) |
			   (ToggleOutcomeIsCC2 << 4) | (ToggleOutcomeIsSink << 5);
	}

	constexpr static Status1A make(uint8_t raw) {
		return {.RXSOP = Bits<0>(raw),
				.RXSOP1Dbg = Bits<1>(raw),
				.RXSOP2Dbg = Bits<2>(raw),
				.ToggleOutcomeIsCC1 = Bits<3>(raw),
				.ToggleOutcomeIsCC2 = Bits<4>(raw),
				.ToggleOutcomeIsSink = Bits<5>(raw)};
	}
};

struct InterruptA : ReadClear {
	enum : uint8_t { Address = 0x3E };

	uint8_t HardResetRx : 1;
	uint8_t SoftResetRx : 1;
	uint8_t TxSent : 1;
	uint8_t HardResetSent : 1;
	uint8_t RetryFail : 1;
	uint8_t SoftFail : 1;
	uint8_t ToggleDone : 1;
	uint8_t OCPTempEvent : 1;

	constexpr operator uint8_t() {
		return (HardResetRx << 0) | (SoftResetRx << 1) | (TxSent << 2) | (HardResetSent << 3) | (RetryFail << 4) |
			   (SoftFail << 5) | (ToggleDone << 6) | (OCPTempEvent << 7);
	}
	constexpr static InterruptA make(uint8_t raw) {
		return {.HardResetRx = Bits<0>(raw),
				.SoftResetRx = Bits<1>(raw),
				.TxSent = Bits<2>(raw),
				.HardResetSent = Bits<3>(raw),
				.RetryFail = Bits<4>(raw),
				.SoftFail = Bits<5>(raw),
				.ToggleDone = Bits<6>(raw),
				.OCPTempEvent = Bits<7>(raw)};
	}
};

struct InterruptB : ReadClear {
	enum : uint8_t { Address = 0x3F };

	uint8_t GCRSent : 1;
	uint8_t : 7;

	constexpr operator uint8_t() {
		return (GCRSent << 0);
	}
	constexpr static InterruptB make(uint8_t raw) {
		return {.GCRSent = Bits<0>(raw)};
	}
};

struct Status0 : ReadOnly {
	enum : uint8_t { Address = 0x40 };

	uint8_t BCLevel : 2;
	uint8_t Wake : 1;
	uint8_t Alert : 1;
	uint8_t CRCCheck : 1;
	uint8_t Comp : 1;
	uint8_t Activity : 1;
	uint8_t VBusOK : 1;

	constexpr operator uint8_t() {
		return (BCLevel << 0) | (Wake << 2) | (Alert << 3) | (CRCCheck << 4) | (Comp << 5) | (Activity << 6) |
			   (VBusOK << 7);
	}
	constexpr static Status0 make(uint8_t raw) {
		return {.BCLevel = Bits<0, 1>(raw),
				.Wake = Bits<2>(raw),
				.Alert = Bits<3>(raw),
				.CRCCheck = Bits<4>(raw),
				.Comp = Bits<5>(raw),
				.Activity = Bits<6>(raw),
				.VBusOK = Bits<7>(raw)};
	}
};

struct Status1 : ReadOnly {
	enum : uint8_t { Address = 0x41 };

	uint8_t OCP : 1;
	uint8_t OverTemp : 1;
	uint8_t TXFull : 1;
	uint8_t TXEmpty : 1;
	uint8_t RXFull : 1;
	uint8_t RXEmpty : 1;
	uint8_t RXSOP1 : 1;
	uint8_t RXSOP2 : 1;

	constexpr operator uint8_t() {
		return (OCP << 0) | (OverTemp << 1) | (TXFull << 2) | (TXEmpty << 3) | (RXFull << 4) | (RXEmpty << 5) |
			   (RXSOP1 << 6) | (RXSOP2 << 7);
	}
	constexpr static Status1 make(uint8_t raw) {
		return {.OCP = Bits<0>(raw),
				.OverTemp = Bits<1>(raw),
				.TXFull = Bits<2>(raw),
				.TXEmpty = Bits<3>(raw),
				.RXFull = Bits<4>(raw),
				.RXEmpty = Bits<5>(raw),
				.RXSOP1 = Bits<6>(raw),
				.RXSOP2 = Bits<7>(raw)};
	}
};

struct Interrupt : ReadClear {
	enum : uint8_t { Address = 0x42 };

	uint8_t BCLevel : 1;
	uint8_t Collision : 1;
	uint8_t Wake : 1;
	uint8_t Alert : 1;
	uint8_t CRCCheck : 1;
	uint8_t CompChanged : 1;
	uint8_t Activity : 1;
	uint8_t VBusOK : 1;

	constexpr operator uint8_t() {
		return (BCLevel << 0) | (Collision << 1) | (Wake << 2) | (Alert << 3) | (CRCCheck << 4) | (CompChanged << 5) |
			   (Activity << 6) | (VBusOK << 7);
	}
	constexpr static Interrupt make(uint8_t raw) {
		return {.BCLevel = Bits<0>(raw),
				.Collision = Bits<1>(raw),
				.Wake = Bits<2>(raw),
				.Alert = Bits<3>(raw),
				.CRCCheck = Bits<4>(raw),
				.CompChanged = Bits<5>(raw),
				.Activity = Bits<6>(raw),
				.VBusOK = Bits<7>(raw)};
	}
};
} // namespace FUSB302

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
