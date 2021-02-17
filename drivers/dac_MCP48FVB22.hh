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

	enum class Registers : uint8_t {
		DAC0 = 0x00,
		DAC1 = 0x01,
		VREF = 0x08,
		POWER = 0x09,
		GAINSTATUS = 0x0A,
	}
	enum Vref : uint16_t {
		VrefADisabled = 0x0000,
		VrefAInternal = 0x0001,
		VrefAPinUnbuffered = 0x0002,
		VrefAPinBuffered = 0x0003,
		VrefBDisabled = 0x0000,
		VrefBInternal = 0x0001,
		VrefBPinUnbuffered = 0x0002,
		VrefBPinBuffered = 0x0003,
	}


	DacSpi_MCP48FVB22() {
		spi.configure();
		auto pri = System::encode_nvic_priority(SpiConf::priority1, SpiConf::priority2);
		NVIC_SetPriority(SpiConf::IRQn, pri);
		NVIC_EnableIRQ(SpiConf::IRQn);

		spi.disable();
		spi.set_tx_message_size(0);
		spi.enable();
	}

	void init_chip() {
		spi.transmit
	}

	void set_value_blocking(uint32_t chan, uint16_t val) {
	}
	void start() {
		//
	}

	SpiPeriph<SpiConf> spi;
};

