#include <stdint.h> // clangd complains about cstdint????

namespace RockchipPeriph
{
struct I2S {
	uint32_t TXCR;
};

struct I2S_TDM {

	uint32_t TXCR;		 // 0x00 default 0x7200'0000
	uint32_t RXCR;		 // 0x04 default 0x01C8'0000
	uint32_t CKR;		 // 0x08
	uint32_t TXFIFOLR;	 // 0x0C
	uint32_t DMACR;		 // 0x10
	uint32_t INTCR;		 // 0x14 default 0x01F0'0000
	uint32_t INTSR;		 // 0x18
	uint32_t XFER;		 // 0x1C
	uint32_t CLR;		 // 0x20
	uint32_t TXDR;		 // 0x24
	uint32_t RXDR;		 // 0x28
	uint32_t RXFIFOLR;	 // 0x2c
	uint32_t TDM_TXCTRL; // 0x30
	uint32_t TDM_RXCTRL; // 0x34
	uint32_t CLKDIV;	 // 0x38
	uint32_t VERSION;	 // 0x3c
	static constexpr uint32_t Version = 0x013376F1;

	// Setup as 8-channel TX TDM, 24-bit Left Justified I2S
	void tdm_tx8_mode() {
		// Left Justified I2S mode

		enum Path { Path0 = 0, Path1 = 1, Path2 = 2, Path3 = 3 };

		uint32_t t = 0;
		t |= Path0 << 29; //SDO3 to path0
		t |= Path0 << 27; //SDO2 to path0
		t |= Path0 << 25; //SDO1 to path0
		t |= Path0 << 23; //SDO0 to path0

		enum NumChannels { Two = 0, Four = 1, Six = 2, Eight = 3 };
		t |= Eight << 15;

		enum Justify { Right = 0, Left = 1 };
		t |= Left << 12;

		enum FirstBitMode { MSB = 0, LSB = 1 };
		t |= FirstBitMode::MSB;

		enum BusMode { Normal = 0, LeftJust = 1, RightJust = 2 };
		t |= Normal << 9;

		// enum PBM { PCMNoDelay = 0, PCMDelay1 = 1, PCMDelay2 = 2, PCMDelay3 = 3 };
		// t |= PCMNoDelay << 7;

		enum TransferFormat { I2S = 0, PCM = 1, TDM_PCM = 2, TDM_I2S = 2 };
		t |= TDM_I2S << 5;

		// ValidData [16, 32]
		auto valid_data_width = [](unsigned bits) { return bits - 1; };
		t |= valid_data_width(24) << 0;

		TXCR = t;
	}

	// Setup as 6-channel RX TDM, 24-bit Left Justified I2S
	void tdm_rx6_mode() {

		enum Path { Path0 = 0, Path1 = 1, Path2 = 2, Path3 = 3 };

		//TODO constants for shift amount for each reg

		uint32_t t = 0;
		t |= Path0 << 29; //SDI3 from path0
		t |= Path0 << 27; //SDI2 from path0
		t |= Path0 << 25; //SDI1 from path0
		t |= Path0 << 23; //SDI0 from path0

		enum NumChannels { Two = 0, Four = 1, Six = 2, Eight = 3 };
		t |= Six << 15;

		enum Justify { Right = 0, Left = 1 };
		t |= Left << 12;

		enum FirstBitMode { MSB = 0, LSB = 1 };
		t |= FirstBitMode::MSB;

		enum BusMode { Normal = 0, LeftJust = 1, RightJust = 2 };
		t |= Normal << 9;

		enum TransferFormat { I2S = 0, PCM = 1, TDM_PCM = 2, TDM_I2S = 2 };
		t |= TDM_I2S << 5;

		// ValidData [16, 32]
		auto valid_data_width = [](unsigned bits) { return bits - 1; };
		t |= valid_data_width(24) << 0;

		RXCR = t;
	}

	//
	void master_mode() {
		uint32_t t = 0;
		enum LRCK_COMMON { SyncSeparate = 0, SyncBothToTX = 1, SyncBothToRX = 2 };
		t |= SyncBothToTX << 28;
		CKR = t;
	}
};

} // namespace RockchipPeriph

namespace HW
{

// TODO: check which ones are TDM (8chan) and which ones are not (2chan)
static inline volatile RockchipPeriph::I2S *const I2S0 = reinterpret_cast<RockchipPeriph::I2S *>(0xfe400000);
static inline volatile RockchipPeriph::I2S_TDM *const I2S1 = reinterpret_cast<RockchipPeriph::I2S_TDM *>(0xfe410000);
static inline volatile RockchipPeriph::I2S *const I2S2 = reinterpret_cast<RockchipPeriph::I2S *>(0xfe420000);
static inline volatile RockchipPeriph::I2S *const I2S3 = reinterpret_cast<RockchipPeriph::I2S *>(0xfe430000);

} // namespace HW
