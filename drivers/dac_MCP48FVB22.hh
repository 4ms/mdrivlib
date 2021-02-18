#pragma once
#include "arch.hh"
#include "debug.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "spi.hh"
#include "spi_transfer.hh"
#include "spi_transfer_config_struct.hh"

template<typename ConfT>
struct DacSpi_MCP48FVB22 {
	using SpiConf = typename ConfT::SpiConf;
	static constexpr unsigned NumChannelsPerChip = 2;

	DacSpi_MCP48FVB22() {
		latch.high();
		spi.configure();
		auto pri = System::encode_nvic_priority(SpiConf::priority1, SpiConf::priority2);
		NVIC_SetPriority(SpiConf::IRQn, pri);
		NVIC_EnableIRQ(SpiConf::IRQn);

		spi.disable();
	}

	void transmit_open_data32(uint32_t word) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(word);
	}

	void init() {
		spi.disable();
		spi.set_tx_message_size(0);
		spi.enable();
		configure_chip();
	}

	void configure_chip() {
		spi.disable();
		spi.clear_TXTF_flag();
		spi.enable();
		latch.high();
		spi.start_transfer();
		transmit_open_data32(make_packet(VREF, WRITE, VrefB_PinUnbuffered | VrefA_PinUnbuffered));
		transmit_open_data32(make_packet(POWER, WRITE, PowerA_On | PowerB_On));
		transmit_open_data32(make_packet(GAINSTATUS, WRITE, GainB_1x | GainA_1x));
	}

	void set_output_blocking(uint32_t chan, uint16_t val) {
	}

	void start() {
		//
	}

	SpiPeriph<SpiConf> spi;
	typename ConfT::AuxPin latch;

	enum Register : uint8_t {
		DACVALUE0 = 0x00 << 3,
		DACVALUE1 = 0x01 << 3,
		VREF = 0x08 << 3,
		POWER = 0x09 << 3,
		GAINSTATUS = 0x0A << 3,
	};
	enum Command : uint8_t {
		WRITE = 0b110,
		READ = 0b000,
	};
	enum Vref : uint16_t {
		VrefA_Disabled = 0x0000,
		VrefA_Internal = 0x0001,
		VrefA_PinUnbuffered = 0x0002,
		VrefA_PinBuffered = 0x0003,
		VrefB_Disabled = 0x0000,
		VrefB_Internal = 0x0004,
		VrefB_PinUnbuffered = 0x0008,
		VrefB_PinBuffered = 0x000C,
	};
	enum Power : uint16_t {
		PowerA_On = 0x0000,
		PowerA_OffPullDown1k = 0x0001,
		PowerA_OffPullDown100k = 0x0002,
		PowerA_OffFloating = 0x0003,
		PowerB_On = 0x0000,
		PowerB_OffPullDown1k = 0x0004,
		PowerB_OffPullDown100k = 0x0008,
		PowerB_OffFloating = 0x000C,
	};
	enum Gain : uint16_t {
		GainA_2x = (1 << 9),
		GainA_1x = 0x0000,
		GainB_2x = (1 << 8),
		GainB_1x = 0x0000,
		// Todo: R/O BOR bit
	};

	constexpr uint8_t make_command(Register r, Command c) {
		return r | c;
	}
	constexpr uint32_t make_packet(Register r, Command c, uint16_t data) {
		return (make_command(r, c) << 16) | data;
	}
};

