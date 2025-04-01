#pragma once
#include "drivers/periph_base_addr.hh"
#include <cstdint>

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
	// 0x24^^
	uint32_t _pad[52]; //pad from 0x28 to 0xFC

	uint32_t TXDATA[8]; //0x100
};

struct I2C_RXDATA {
	uint32_t RXDATA[8];
};

struct I2C_DEBUG {
	uint32_t ST;	  // status debug
	uint32_t DBGCTRL; // debug config
};

} // namespace RockchipPeriph

struct I2C_Hal {
	volatile RockchipPeriph::I2C *Instance;
};

inline volatile RockchipPeriph::I2C *I2C0 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C0_BASE);
inline volatile RockchipPeriph::I2C *I2C1 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C1_BASE);
inline volatile RockchipPeriph::I2C *I2C2 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C2_BASE);
inline volatile RockchipPeriph::I2C *I2C3 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C3_BASE);
inline volatile RockchipPeriph::I2C *I2C4 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C4_BASE);
inline volatile RockchipPeriph::I2C *I2C5 = reinterpret_cast<volatile RockchipPeriph::I2C *>(I2C5_BASE);

inline volatile RockchipPeriph::I2C *TXDATA(volatile RockchipPeriph::I2C *i2c) {
	return i2c + 0x100;
}

} // namespace mdrivlib
