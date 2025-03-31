#include "drivers/periph_base_addr.hh"
#include <cstdint>
#include <cstdio>

namespace mdrivlib
{

namespace RockchipPeriph
{

// I2S clock tree:
// hclk_i2s1_8ch <-- hclk_gic_audio <-- gpll_100m (selectable) <--- gpll <--- xin 24MHz
//
// hclk_i2s1_8ch: Gate is CRU_GATE_CON05 at bit 11
//
// hclk_gic_audio: CRU_CLKSEL_CON10 bit 10 to select gpll_150m or gpll_100m etc; CRU_GATE_CON05 bit 1
//
// gpll_100m: Gate is CON35 bit 4, Divider is CLKSEL_CON77 bits 4:0
// gpll_150m ...TODO...
//
// gpll: CRU_GPLL_CON0 @ 0xfdd20040, CRU_MODE_CON00 (@ 0xfdd200c0) bits 7:6 need to be 0b01
// gpll is set to 1.2GHz with:
//   GPLL_CON0 is 0x2064 => fbdiv=0x64=100d, postdiv=2, no bypass. So 24MHz*100 = 2.4GHz, /2 = 1.2GHz
//   GPLL_CON1 is 0x1441 => refdiv=1, postdiv2=1, locked, modulator disabled, no power down

// mclk clock tree:
// mclk_i2s1_8ch_tx <-- clk_i2s1_8ch_tx <-- selectable: clk_i2s1_8ch_tx_src, clk_i2s1_8ch_tx_frac, i2s1_mclk_in <-- gpll
//
// mclk_i2s1_8ch_tx: GATE_CON06, bit 10
//
// clk_i2s1_8ch_tx:
//			CRU_CLKSEL_CON15, bit 15  set to 0 to enable
//			CRU_CLKSEL_CON15, bits 11:10  set to 0b00 to use integer divider
//
// clk_i2s1_8ch_tx_src (integer divider clock):
//			CRU_CLKSEL_CON15, bits 9:8  set to 0b00 to use gpll
//			CRU_CLKSEL_CON15, bits 6:0  selects the divider for gpll (e.g. /98)
//			CRU_GATE_CON06, bit 11 cleared = enable mclkout_tx

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

	bool check_version() {
		return VERSION == Version;
	}

	void reset() volatile {
		XFER = 0;
		CLR = 1;
		while (CLR != 0)
			;
	}

	void start_tx() volatile {
		XFER = 0b01;
	}

	void start_txrx() volatile {
		XFER = 0b11;
	}

	void start_rx() volatile {
		XFER = 0b10;
	}

	// Setup as 8-channel TX TDM, 24-bit Left Justified I2S
	void tdm_tx8_mode() volatile {

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

		printf("Setting TXCR %p to %08x\n", &TXCR, t);
		// 00019057
		TXCR = t;

		t = 0;

		// TODO: use enums instead of magic numbers:
		t |= 0 << 17;	  // fsync is half frame
		t |= 0b000 << 14; // I2S format 0 (normal)
		t |= 0x1f << 9;	  // TX Slot bit width (32)
		t |= 0xff << 0;	  // TX Frame width (256)

		printf("Setting TDM_TXCTRL %p to %08x\n", &TDM_TXCTRL, t);
		TDM_TXCTRL = t;

		t = 0;
		t |= 0xff << 8; // RX:
		t |= 0xff << 0; // TX:divide MCLK by 1 to get SCLK

		printf("Setting CLKDIV %p to %08x\n", &CLKDIV, t);
		CLKDIV = t;
	}

	// setup as 8channel non-tdm
	void tx8_parallel_mode() volatile {

		enum Path { Path0 = 0, Path1 = 1, Path2 = 2, Path3 = 3 };

		uint32_t t = 0;
		t |= Path3 << 29; //SDO3 to path3
		t |= Path2 << 27; //SDO2 to path2
		t |= Path1 << 25; //SDO1 to path1
		t |= Path0 << 23; //SDO0 to path0

		enum NumChannels { Two = 0, Four = 1, Six = 2, Eight = 3 };
		t |= Two << 15;

		enum Justify { Right = 0, Left = 1 };
		t |= Left << 12;

		enum FirstBitMode { MSB = 0, LSB = 1 };
		t |= FirstBitMode::MSB;

		enum BusMode { Normal = 0, LeftJust = 1, RightJust = 2 };
		t |= Normal << 9;

		// enum PBM { PCMNoDelay = 0, PCMDelay1 = 1, PCMDelay2 = 2, PCMDelay3 = 3 };
		// t |= PCMNoDelay << 7;

		enum TransferFormat { I2S = 0, PCM = 1, TDM_PCM = 2, TDM_I2S = 2 };
		t |= I2S << 5;

		// ValidData [16, 32]
		auto valid_data_width = [](unsigned bits) { return bits - 1; };
		t |= valid_data_width(24) << 0;

		printf("Setting TXCR %p to %08x\n", &TXCR, t);
		TXCR = t;

		t = 0;
		// TODO: use enums instead of magic numbers:
		t |= 0 << 17;	  // fsync is half frame
		t |= 0b000 << 14; // I2S format 0 (normal)
		t |= 0x1f << 9;	  // TX Slot bit width (32)
		t |= 0x1f << 0;	  // TX Frame width (64)?

		printf("Setting TDM_TXCTRL %p to %08x\n", &TDM_TXCTRL, t);
		TDM_TXCTRL = t;

		t = 0;
		t |= 0x3 << 8; // RX:
		t |= 0x3 << 0; // TX:divide MCLK by 4 to get SCLK

		printf("Setting CLKDIV %p to %08x\n", &CLKDIV, t);
		CLKDIV = t;
	}
	// Setup as 6-channel RX TDM, 24-bit Left Justified I2S
	void tdm_rx6_mode() volatile {

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

		printf("Setting RXCR %p to %08x\n", &RXCR, t);
		RXCR = t;
	}

	// TX is the master channel. TX sends LR CLK which RX channel syncs to
	void master_tx() volatile {
		uint32_t t = 0;

		// TODO: not sure what this one does
		// Linux driver uses SeparateLRClks, but we might want to share one LR line
		enum LRCK_COMMON { SeparateLRClks = 0b00, SyncToTx = 0b01, SyncToRx = 0b10 };
		t |= SeparateLRClks << 28;

		enum MSS { Master = 0, Slave = 1 };
		t |= Master << 27;

		enum CKP { SclkPolarityNormal = 0, SclkPolarityInverted = 1 };
		t |= SclkPolarityNormal << 26;

		enum RLP { RxLrclkPolarityNormal = 0, RxLrclkPolarityInverted = 1 };
		t |= RxLrclkPolarityNormal << 25;

		enum TLP { TxLrclkPolarityNormal = 0, TxLrclkPolarityInverted = 1 };
		t |= TxLrclkPolarityNormal << 24;

		// RSD: RX Sclk Divider
		// FreqSclk = (DIV / 2) * 2 * FreqLRClk
		//      DIV = FreqSclk / FreqLRClk
		// Only matters in non-TDM mode
		t |= 0x3f << 8;

		// TSD: Transmit Sclk Divider
		t |= 0x3f << 0;

		printf("Setting CKR %p to %08x\n", &CKR, t);
		// 0x1000ffff
		CKR = t;
	}

	void enable_DMA() volatile {
		uint32_t t = 0;
		enum { RXDMAEnableBit = 24, RXDMALevelShift = 16, TXDMAEnableBit = 8, TXDMALevelShift = 0 };

		constexpr uint32_t LevelMask = 0b11111;

		t |= 1 << RXDMAEnableBit;
		t |= 1 << TXDMAEnableBit;

		// "level at which a DMA request is made by the receive logic."
		t |= (16 | LevelMask) << RXDMALevelShift;
		t |= (16 | LevelMask) << TXDMALevelShift;

		printf("Setting DMACR %p to %08x\n", &DMACR, t);
		DMACR = t;
	}

	void clear_tx_underrun() volatile {
		enum Bits { TXEmpty = 0, TXUnderrun = 1 };
		if (INTSR & (1 << TXUnderrun)) {
			uint32_t t = INTCR;
			t |= 1 << 2;
			// printf("Clear underrun: Setting INTCR %p to %08x\n", &INTCR, t);
			INTCR = t;
		}
		if (INTSR & (1 << TXEmpty)) {
			// printf("TX empty\n");
		}
	}

	void enable_TX_ISR_with_block_size(uint32_t block_size) volatile {
		uint32_t t = INTCR;

		enum Bits { TXEmptyInterruptEnable = 0, TXUnderrunInterruptEnable = 1, TXFIFOThreshold = 4 };
		enum Mask { TXFIFOThresholdMask = 0b11111 }; //31 max

		t |= 0 << TXFIFOThreshold;
		t |= 1 << TXEmptyInterruptEnable;
		t |= 0 << TXUnderrunInterruptEnable;
		t |= ((block_size - 1) & TXFIFOThresholdMask) << TXFIFOThreshold;

		printf("Setting INTCR %p to %08x\n", &INTCR, t);
		INTCR = t;
	}
};

struct I2S_2CH {
	//TODO
};

} // namespace RockchipPeriph

static inline volatile RockchipPeriph::I2S_TDM *const I2S0 = reinterpret_cast<RockchipPeriph::I2S_TDM *>(I2S0_8CH_BASE);
static inline volatile RockchipPeriph::I2S_TDM *const I2S1 = reinterpret_cast<RockchipPeriph::I2S_TDM *>(I2S1_8CH_BASE);
static inline volatile RockchipPeriph::I2S_2CH *const I2S2 = reinterpret_cast<RockchipPeriph::I2S_2CH *>(I2S2_2CH_BASE);
static inline volatile RockchipPeriph::I2S_2CH *const I2S3 = reinterpret_cast<RockchipPeriph::I2S_2CH *>(I2S3_2CH_BASE);

} // namespace mdrivlib
