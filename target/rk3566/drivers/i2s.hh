#include <cstdio>
#include <stdint.h> // clangd complains about cstdint????

namespace RockchipPeriph
{
struct I2S {
	uint32_t TXCR;
	//TODO... I2S 2 channel registers
};

// hclk_i2s1_8ch <-- hclk_gic_audio <-- gpll_100m (selectable)
//
// GATE(HCLK_I2S1_8CH, "hclk_i2s1_8ch", "hclk_gic_audio", 0, RK3568_CLKGATE_CON(5), 11, GFLAGS),
// { .id = HCLK_I2S1_8CH == 58,
//   .branch_type = branch_gate,
//   .name = "hclk_i2s1_8ch"
//   .parent_names = "hclk_gic_audio",
//   .num_parents = 1,
//   .flags = 0,
//   .gate_offset = RK3568_CLKGATE_CON(5), == 5*0x4 + 0x300 = 0x314  is CRU_GATE_CON05
//   .gate_shift = 11,
//   .gate_flags = GFLAGS == (CLK_GATE_HIWORD_MASK | CLK_GATE_SET_TO_DISABLE)

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
// Gate is CON35 bit 4, Divider is CLKSEL_CON77 bits 4:0 (default 0x0b, confirmed they are that after uboot)

// mclk_i2s1_8ch_tx <-- clk_i2s1_8ch_tx <--
// 	GATE(MCLK_I2S1_8CH_TX, "mclk_i2s1_8ch_tx", "clk_i2s1_8ch_tx", 0, RK3568_CLKGATE_CON(6), 10, GFLAGS),
//  GATE_CON06, bit 10

//	COMPOSITE_NODIV(I2S1_MCLKOUT_TX, "i2s1_mclkout_tx", i2s1_mclkout_tx_p, CLK_SET_RATE_PARENT,
//			RK3568_CLKSEL_CON(15), 15, 1, MFLAGS,
//			RK3568_CLKGATE_CON(6), 11, GFLAGS),

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
		t |= 255 << 8;

		// TSD: Transmit Sclk Divider
		t |= 255 << 0;

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
