#pragma once
#include "drivers/dma_config_struct.hh"
#include "drivers/interrupt.hh"
#include "drivers/memory_transfer.hh"
#include "drivers/spi.hh"
#include "util/math.hh"
#include <functional>
#include <span>

namespace mdrivlib
{

// Todo: pass dma as reference in ctor, rather than passing its type in the template params?
template<typename ConfT, typename DmaTransferT>
struct SpiDmaDataCmdDriver {
	using ScreenConfT = ConfT::ScreenSpiConf;

	enum PacketType { Cmd, Data };

	SpiDmaDataCmdDriver() = default;

	void init() {
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(1); // not needed?
		spi.enable();

		spi_dr = spi.get_tx_datareg_addr();
	}

	void reinit() {
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(1); // not needed?
		spi.enable();
		dma.init();
	}

	// Clears the OVR flag whenever its set, by enabling the IRQ
	void clear_overrun_on_interrupt() {
		InterruptControl::disable_irq(ScreenConfT::IRQn);
		spi.clear_overrun_flag();
		InterruptManager::register_and_start_isr(
			ScreenConfT::IRQn, ScreenConfT::priority1, ScreenConfT::priority2, [this]() {
			if (spi.is_overrun())
				spi.clear_overrun_flag();
		});
	}

	template<PacketType MessageType>
	void transmit_blocking(uint8_t byte) {
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.set_tx_message_size(1);
		spi.set_data_size(8);
		spi.set_fifo_threshold(1);
		spi.disable_end_of_xfer_interrupt();
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();
		spi.start_transfer();
		spi.load_tx_data(byte);
		while (!spi.is_tx_complete())
			;
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
	}

	template<PacketType MessageType>
	void transmit_blocking(uint16_t halfword1, uint16_t halfword2) {
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.set_data_size(8);
		spi.set_tx_message_size(4);
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes_and_combine(halfword2, halfword1));
		spi.start_transfer();
		while (!spi.is_tx_complete())
			;
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
	}

	// Max 64kb
	template<PacketType MessageType>
	void transmit_blocking(std::span<const uint8_t> bytes) {
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();

		spi.set_tx_message_size(std::min<uint16_t>(0xFFFF, bytes.size()));
		spi.set_data_size(8);

		spi.set_fifo_threshold(1);
		// spi.set_fifo_threshold(std::min(bytes.size(), 8));

		spi.disable_end_of_xfer_interrupt();
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();

		for (auto b : bytes) {
			spi.start_transfer();
			spi.load_tx_data_8(b);
			while (!spi.tx_space_available())
				;
		}

		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
	}

	void config_dma_transfer(uint32_t src, uint32_t sz) {
		// Todo: we could check if data_size doesn't match conf data size, thus skipping the need to disable/enable the
		// SPI
		if constexpr (ConfT::DMAConf::transfer_size_periph == ConfT::DMAConf::HalfWord) {
			spi.disable();
			spi.set_data_size(16);
			spi.enable();
		} else if (ConfT::DMAConf::transfer_size_periph == ConfT::DMAConf::Word) {
			spi.disable();
			spi.set_data_size(32);
			spi.enable();
		}

		dma.config_transfer(spi_dr, src, sz);
	}

	void register_callback(auto cb) {
		dma.register_callback(cb);
	}

	void start_dma_transfer(auto cb) {
		dma.register_callback(cb);
		start_dma_transfer();
	}

	void start_dma_transfer() {
		spi.disable();
		dma.start_transfer();

		uint32_t sz = dma.get_transfer_size();
		if (sz <= 0xFFFF) {
			spi.set_tx_message_size(sz);
			spi.set_tx_message_reload_size(0);
		} else if (sz <= (0xFFFF * 2)) {
			spi.set_tx_message_size(0xFFFF);
			spi.set_tx_message_reload_size(sz - 0xFFFF);
		} else
			return; // Todo: handle transfers > 128k packets by setting up an ISR on TSERF that sets the reload size

		dcpin.high();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.enable_tx_dma();
		spi.enable();
		spi.start_transfer();
	}

	bool had_transfer_error() {
		return dma.had_transfer_error();
	}

	bool had_fifo_error() {
		return dma.had_fifo_error();
	}

private:
	SpiPeriph<ScreenConfT> spi;
	typename ConfT::DCPin dcpin;

	DmaTransferT dma;
	uint32_t spi_dr;

	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_te_flag_index;
};
} // namespace mdrivlib
