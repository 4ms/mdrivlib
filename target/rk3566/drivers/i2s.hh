#include <cstdio>
#include <stdint.h> // clangd complains about cstdint????

namespace RockchipPeriph
{

// hclk_i2s1_8ch <-- hclk_gic_audio <-- gpll_100m (selectable) <--- gpll <--- xin 24MHz
//
// hclk_i2s1_8ch:
// CLKGATE_CON(5) is CRU_GATE_CON05 at bit 11
//
// parent clock is hclk_gic_audio:
// 	COMPOSITE_NODIV(HCLK_GIC_AUDIO, "hclk_gic_audio", gpll150_gpll100_gpll75_xin24m_p, CLK_IGNORE_UNUSED,
//			RK3568_CLKSEL_CON(10), 10, 2, MFLAGS,
//			RK3568_CLKGATE_CON(5), 1, GFLAGS),
// parent_names = gpll150_gpll100_gpll75_xin24m_p
// .muxdiv_offset = RK3568_CLKSEL_CON(10)
// .mux_shift = 10
// .mux_width = 2
// .mux_flags = MFLAGS == CLK_MUX_HIWORD_MASK
// gate = CON05 bit 1

// parent is selectable, e.g. gpll_100m
// gpll_100m Gate is CON35 bit 4,
// gpll_100m Divider is CLKSEL_CON77 bits 4:0 (default 0x0b, confirmed they are that after uboot)

// parent is gpll, which is a pll_rk3328
// PLL_CON16 (offset 16*4 = 0x0040) is CRU_GPLL_CON0 @ 0xfdd20040
// CRU_MODE_CON00 (@ 0xfdd200c0) bits 7:6 need to be 0b01 (they are after uboot: 0xfdd200c0 is 0x0055)
// in dtsi, gpll is set to 1.2GHz
// GPLL_CON0 is 0x2064 => fbdiv=0x64=100d, postdiv=2, no bypass. So 24MHz*100 = 2.4GHz, /2 = 1.2GHz
// GPLL_CON1 is 0x1441 => refdiv=1, postdiv2=1, locked, modulator disabled, no power down
//
// parent is xin24m

// mclk_i2s1_8ch_tx <-- clk_i2s1_8ch_tx <-- selecctable: clk_i2s1_8ch_tx_src, clk_i2s1_8ch_tx_frac, i2s1_mclk_in
//
// mclk_i2s1_8ch_tx:
//  GATE_CON06, bit 10
//
// parent is clk_i2s1_8ch_tx:
//	COMPOSITE_NODIV(I2S1_MCLKOUT_TX, "i2s1_mclkout_tx", i2s1_mclkout_tx_p, CLK_SET_RATE_PARENT,
//			RK3568_CLKSEL_CON(15), bit 15 (rr 0xfdd2013c -> 0x00000113 or also tried 0x0013). means /20 --> 60MHz
//											also tried 0x0032 means /50 -> 24MHz
//			RK3568_CLKGATE_CON(6), bit 11 (rr 0xfdd20318 -> 0x00000000)
//
// parent is gpll_cpll_npll_p: which we can select gpll, which we know is 1.2GHz
//

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
		// I think this means that DIV is a ratio:
		//      Ratio = FreqSclk / FreqLRClk
		// Only matters in non-TDM mode
		t |= 0x1f << 8;

		// TSD: Transmit Sclk Divider
		t |= 0x1f << 0;

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
			printf("Clear underrun: Setting INTCR %p to %08x\n", &INTCR, t);
			INTCR = t;
		}
		if (INTSR & (1 << TXEmpty)) {
			printf("TX empty\n");
		}
	}

	void enable_ISR() volatile {
		uint32_t t = INTCR;

		enum Bits { TXEmptyInterruptEnable = 0, TXUnderrunInterruptEnable = 1, TXFIFOThreshold = 4 };
		enum Mask { TXFIFOThresholdMask = 0b11111 };

		t |= 1 << TXEmptyInterruptEnable;
		t |= 0 << TXUnderrunInterruptEnable;
		t |= (1 & TXFIFOThresholdMask) << TXFIFOThreshold;

		printf("Setting INTCR %p to %08x\n", &INTCR, t);
		INTCR = t;
	}
};

} // namespace RockchipPeriph

namespace HW
{

// TODO: check which ones are TDM (8chan) and which ones are not (2chan)
// static inline volatile RockchipPeriph::I2S *const I2S0 = reinterpret_cast<RockchipPeriph::I2S *>(0xfe400000);
static inline volatile RockchipPeriph::I2S_TDM *const I2S1 = reinterpret_cast<RockchipPeriph::I2S_TDM *>(0xfe410000);
// static inline volatile RockchipPeriph::I2S *const I2S2 = reinterpret_cast<RockchipPeriph::I2S *>(0xfe420000);
// static inline volatile RockchipPeriph::I2S *const I2S3 = reinterpret_cast<RockchipPeriph::I2S *>(0xfe430000);

} // namespace HW
