#pragma once
#include "drivers/periph_base_addr.hh"
#include <cstdint>
#include <cstdio>
#include <utility>

namespace mdrivlib
{

namespace RockchipPeriph
{

// Note: CM3 board has pullups on:
// GPIO0-B1/B2 (I2C0 for PMIC)
// GPIO0-B5/B6 (I2C2_M0)
// GPIO4-B4/B5 (I2C2_M1)
// GPIO1-A0/A1 (I2C3 for codec)

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

	//
	// TX/RX
	//

	enum class CON_bits {
		enable = (1 << 0),
		mode_shift = 1,
		mode = (0b11 << mode_shift),
		start = (1 << 3),
		stop = (1 << 4),
		ack = (1 << 5),
		act2nak = (1 << 6),
		data_upd_st = (1 << 8),
		start_setup = (1 << 12),
		stop_setup = (1 << 14)
	};
	void enable() volatile {
		CON = std::to_underlying(CON_bits::enable);
	}
	void disable() volatile {
		CON = 0;
	}
	void tx_mode() volatile {
		CON = (0b00 << std::to_underlying(CON_bits::mode_shift)) | std::to_underlying(CON_bits::enable);
	}
	void rx_mode() volatile {
		CON = (0b10 << std::to_underlying(CON_bits::mode_shift)) | std::to_underlying(CON_bits::enable);
	}
	//TODO: what is the difference between txrx_mode1 and txrx_mode2?
	void txrx_mode1() volatile {
		CON = (0b01 << std::to_underlying(CON_bits::mode_shift)) | std::to_underlying(CON_bits::enable);
	}
	void txrx_mode2() volatile {
		CON = (0b11 << std::to_underlying(CON_bits::mode_shift)) | std::to_underlying(CON_bits::enable);
	}
	void send_start() volatile {
		CON = std::to_underlying(CON_bits::start) | std::to_underlying(CON_bits::enable);
	}
	void send_ack() volatile {
		CON = std::to_underlying(CON_bits::ack) | std::to_underlying(CON_bits::enable);
	}
	void send_stop() volatile {
		CON = std::to_underlying(CON_bits::stop) | std::to_underlying(CON_bits::enable);
	}

	void tx_data(unsigned wordnum, uint32_t val) volatile {
		TXDATA[wordnum] = val;
	}
	//
	// Events/interrupts
	//

	enum class EventBits {
		byte_tx_finished = (1 << 0),
		byte_rx_finished = (1 << 1),
		mtxcnt_tx_finished = (1 << 2),
		mrxcnt_rx_finished = (1 << 3),
		start_finished = (1 << 4),
		stop_finished = (1 << 5),
		nak_received = (1 << 6),
		slave_hold_scl = (1 << 7),
		all = 0xFF,
	};

	bool is_pending(EventBits bit) volatile {
		return IPD & std::to_underlying(bit);
	}
	void clear_all_pending() volatile {
		IPD = std::to_underlying(EventBits::all);
	}
	void clear_pending(EventBits bit) volatile {
		IPD = std::to_underlying(bit);
	}
	void enable_events(auto... bit) volatile {
		IEN = (std::to_underlying(bit) | ...);
	}

	//
	// Higher-level control
	//

	// returns true if bit was eventually set, false if timed out
	bool wait_for_pending_bit(EventBits bit) volatile {
		uint32_t timeout = 1000000; // maybe ~10ms?
		while ((is_pending(bit)) == 0) {
			if (timeout-- == 0) {
				printf("timeout: IPD=%x\n", IPD);
				return false;
			}
		}
		clear_pending(bit);
		return true;
	}

	bool perform_start() volatile {
		clear_all_pending();
		enable_events(EventBits::start_finished);
		send_start();
		//TODO: set enable IE for start??
		return wait_for_pending_bit(EventBits::start_finished);
	}

	bool perform_stop() volatile {
		clear_all_pending();
		send_stop();
		//TODO: set enable IE for start??
		return wait_for_pending_bit(EventBits::stop_finished);
	}

	bool transmit(unsigned size) volatile {
		printf("Transmit %u\n", size);

		enable_events(EventBits::mtxcnt_tx_finished, EventBits::nak_received);

		MTXCNT = size;

		uint32_t timeout = 1000000; // maybe ~10ms?
		while (true) {
			if (is_pending(EventBits::mtxcnt_tx_finished)) {
				clear_pending(EventBits::mtxcnt_tx_finished);
				return true;
			}
			if (is_pending(EventBits::nak_received)) {
				clear_pending(EventBits::nak_received);
				return false;
			}
			if (timeout-- == 0) {
				printf("timeout: IPD=%x\n", IPD);
				return false;
			}
		}
	}

	//
	// Bus Config
	//

	void clock_div(uint16_t SCL_high, uint16_t SCL_low) volatile {
		CLKDIV = ((uint32_t)SCL_high << 16) | SCL_low;
		// Time SCL high = Tclk_i2c * (SCL_high + 1)  * 8;
		// Time SCL low = Tclk_i2c * (SCL_low + 1)  * 8;
		// Period = Tclk_i2c * (SCL_high + SCL_low + 2) * 8;
	}

	void set_slave_hold_debounce(uint8_t cycles) {
		SCL_OE_DB = cycles;
	}

	enum class DBGCTRL_bits {
		filter_fall = (0b1111 << 0),
		filter_rise = (0b1111 < 4),
		slave_hold_thresh = (0b1111 << 8),
		glitch_filter = 1 << 12,
		nack_release_scl_high = 1 << 13,
		h0_check_scl = 1 << 14,
	};

	void enable_edge_glitch_filter() volatile {
		DBGCTRL = DBGCTRL | std::to_underlying(DBGCTRL_bits::glitch_filter);
	}
	void disable_edge_glitch_filter() volatile {
		DBGCTRL = DBGCTRL & ~std::to_underlying(DBGCTRL_bits::glitch_filter);
	}
	void h0_check_scl() volatile {
		DBGCTRL = DBGCTRL | std::to_underlying(DBGCTRL_bits::h0_check_scl);
	}
	void all_check_scl() volatile {
		DBGCTRL = DBGCTRL & ~std::to_underlying(DBGCTRL_bits::h0_check_scl);
	}
	void nack_release_scl_high() volatile {
		DBGCTRL = DBGCTRL | std::to_underlying(DBGCTRL_bits::nack_release_scl_high);
	}
	void nack_release_scl_low() volatile {
		DBGCTRL = DBGCTRL & ~std::to_underlying(DBGCTRL_bits::nack_release_scl_high);
	}
	void set_slave_hold_scl_threshold(uint8_t th) volatile {
		auto mask = std::to_underlying(DBGCTRL_bits::slave_hold_thresh);
		auto bits = (th << 8) & mask;
		DBGCTRL = (DBGCTRL & ~mask) | bits;
	}
	void filter_falling_edges(uint8_t min_width) volatile {
		auto mask = std::to_underlying(DBGCTRL_bits::filter_fall);
		auto bits = (min_width << 0) & mask;
		DBGCTRL = (DBGCTRL & ~mask) | bits;
	}
	void filter_rising_edges(uint8_t min_width) volatile {
		auto mask = std::to_underlying(DBGCTRL_bits::filter_rise);
		auto bits = (min_width << 4) & mask;
		DBGCTRL = (DBGCTRL & ~mask) | bits;
	}

	void dump_registers() {
		printf("i2c_con: 0x%08x\n", (CON));
		printf("i2c_clkdiv: 0x%08x\n", (CLKDIV));
		printf("i2c_mrxaddr: 0x%08x\n", (MRXADDR));
		printf("i2c_mrxraddR: 0x%08x\n", (MRXRADDR));
		printf("i2c_mtxcnt: 0x%08x\n", (MTXCNT));
		printf("i2c_mrxcnt: 0x%08x\n", (MRXCNT));
		printf("i2c_ien: 0x%08x\n", (IEN));
		printf("i2c_ipd: 0x%08x\n", (IPD));
		printf("i2c_fcnt: 0x%08x\n", (FCNT));
		for (int i = 0; i < 8; i++)
			printf("i2c_txdata%d: 0x%08x\n", i, (TXDATA[i]));
		for (int i = 0; i < 8; i++)
			printf("i2c_rxdata%d: 0x%08x\n", i, (RXDATA[i]));
	}
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
