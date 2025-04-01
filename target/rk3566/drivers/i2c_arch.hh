#pragma once
#include "drivers/periph_base_addr.hh"
#include <cstdint>
#include <utility>

namespace mdrivlib
{

namespace RockchipPeriph
{

struct I2C {
	uint32_t CON;		// control register
	uint32_t CLKDIV;	// clock diver PCLK / (16*CLKDIV)
	uint32_t MRXADDR;	// slave address accessed for master rx mode
	uint32_t MRXRADDR;	// slave register address accessed for master rx mode
	uint32_t MTXCNT;	//master transmit count; specify total number of bytes to be transmit (0-32)
	uint32_t MRXCNT;	//master rx count; specify total number of bytes to be received (0-32)
	uint32_t IEN;		//interrupt enable register
	uint32_t IPD;		//interrupt pending
	uint32_t FCNT;		//finished count
	uint32_t SCL_OE_DB; //slave hold debounce configure

	uint32_t _pad[(0x100 - 0x28) / 4]; //0x28 is offset of SCL_OE_DB

	uint32_t TXDATA[8]; //0x100

	uint32_t _pad2[((0x200 - 0x100) / 4) - 8];

	uint32_t RXDATA[8]; //0x200

	uint32_t ST;	  // status debug
	uint32_t DBGCTRL; // debug config

	enum class CON_bits {
		enable = 0,
		mode = 1,
		start = 3,
		stop = 4,
		ack = 5,
		act2nak = 6,
		data_upd_st = 8,
		start_setup = 12,
		stop_setup = 14
	};
	void enable() volatile {
		CON = CON | (1 << std::to_underlying(CON_bits::enable));
	}
	void tx_mode() volatile {
		CON = CON | (0b00 << std::to_underlying(CON_bits::mode));
	}
	void rx_mode() volatile {
		CON = CON | (0b10 << std::to_underlying(CON_bits::mode));
	}
	//TODO: what is the difference between txrx_mode1 and txrx_mode2?
	void txrx_mode1() volatile {
		CON = CON | (0b01 << std::to_underlying(CON_bits::mode));
	}
	void txrx_mode2() volatile {
		CON = CON | (0b11 << std::to_underlying(CON_bits::mode));
	}
	void send_start() volatile {
		CON = CON | (1 << std::to_underlying(CON_bits::start));
	}
	void send_ack() volatile {
		CON = CON | (1 << std::to_underlying(CON_bits::ack));
	}
	void send_stop() volatile {
		CON = CON | (1 << std::to_underlying(CON_bits::stop));
	}

	void clock_div(uint16_t SCL_high, uint16_t SCL_low) volatile {
		CLKDIV = ((uint32_t)SCL_high << 16) | SCL_low;
		// Time SCL high = Tclk_i2c * (SCL_high + 1)  * 8;
		// Time SCL low = Tclk_i2c * (SCL_low + 1)  * 8;
		// Period = Tclk_i2c * (SCL_high + SCL_low + 2) * 8;
	}

	enum class IPD_bits {
		mtxcnt_tx_finished = (1 << 2),
	};
};

#ifndef __clang__
static_assert(offsetof(I2C, TXDATA) == 0x100);
static_assert(offsetof(I2C, RXDATA) == 0x200);
static_assert(offsetof(I2C, ST) == 0x220);
#endif

} // namespace RockchipPeriph

struct I2C_Hal {
	volatile RockchipPeriph::I2C *instance;
};

inline volatile RockchipPeriph::I2C *I2C0 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C0_BASE);
inline volatile RockchipPeriph::I2C *I2C1 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C1_BASE);
inline volatile RockchipPeriph::I2C *I2C2 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C2_BASE);
inline volatile RockchipPeriph::I2C *I2C3 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C3_BASE);
inline volatile RockchipPeriph::I2C *I2C4 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C4_BASE);
inline volatile RockchipPeriph::I2C *I2C5 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C5_BASE);

} // namespace mdrivlib
