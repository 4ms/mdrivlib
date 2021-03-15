#pragma once
#include "debug.hh" //FixMe: remove this when done debugging
#include "dma_config_struct.hh"
#include "drivers/bdma.hh"
#include "drivers/memory_transfer.hh"
#include "interrupt.hh"
#include "spi.hh"
#include "system.hh"
#include "util/math.hh"

// Todo: this is not necessarily an SPI screen driver, it's a general SPI driver that has a hi/low pin for each transfer
// and it's blocking mode only
template<typename ConfT>
struct SpiScreenDriver {
	SpiScreenDriver()
		: _ready(true) {
		InterruptManager::registerISR(
			ConfT::ScreenSpiConf::IRQn, ConfT::ScreenSpiConf::priority1, ConfT::ScreenSpiConf::priority2, [this]() {
				if (spi.is_end_of_transfer()) {
					spi.clear_EOT_flag();
					spi.clear_TXTF_flag();
					_ready = true;
				}
			});

		spi.disable();
		spi.configure();
		NVIC_EnableIRQ(ConfT::ScreenSpiConf::IRQn);

		spi.set_tx_message_size(1);
		spi.enable();
		spi.enable_end_of_xfer_interrupt();
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;
	volatile bool _ready;
	// uint32_t _data_ctr;
	void wait_until_ready() {
		while (!_ready) {
		}
		_ready = false;
	}

protected:
	// Blocking Mode:
	enum PacketType { Cmd, Data };

	template<PacketType MessageType>
	void transmit(uint8_t byte) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(1);
		spi.enable();
		if constexpr (MessageType == Cmd)
			dcpin.low();
		if constexpr (MessageType == Data)
			dcpin.high();
		spi.load_tx_data(byte);
		spi.start_transfer();
	}

	void transmit_data_16(uint16_t halfword) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(2);
		spi.enable();
		dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes16(halfword));
		spi.start_transfer();
	}

	void transmit_data_32(uint16_t halfword1, uint16_t halfword2) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(4);
		spi.enable();
		dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes_and_combine(halfword2, halfword1));
		spi.start_transfer();
	}

	void transmit_data_32(uint32_t word) {
		wait_until_ready();
		spi.disable();
		spi.set_tx_message_size(4);
		spi.enable();
		dcpin.high();
		spi.load_tx_data(MathTools::swap_bytes32(word));
		spi.start_transfer();
	}

	void begin_open_data_transmission(uint8_t fifo_size) {
		wait_until_ready();
		while (!spi.tx_space_available())
			;
		spi.disable();
		spi.set_tx_message_size(0);
		spi.set_fifo_threshold(fifo_size);
		spi.disable_end_of_xfer_interrupt();
		spi.clear_TXTF_flag();
		spi.enable();
		dcpin.high();
		spi.start_transfer();
	}

	void transmit_open_data16(uint16_t halfword) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(halfword);
	}

	void transmit_open_data32(uint16_t halfword1, uint16_t halfword2) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(MathTools::swap_bytes_and_combine(halfword2, halfword1));
	}

	void transmit_open_data32(uint32_t word) {
		while (!spi.tx_space_available())
			;
		spi.load_tx_data(MathTools::swap_bytes32(word));
	}

	void end_open_data_transmission() {
		spi.disable();
		spi.set_tx_message_size(1);
		spi.set_fifo_threshold(1);
		spi.enable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		_ready = true;
		spi.enable_end_of_xfer_interrupt();
	}
};

////////////////////////
// DmaSpiScreenDriver //
////////////////////////
template<typename ConfT>
struct DmaSpiScreenDriver {
	enum PacketType { Cmd, Data };

	DmaSpiScreenDriver() {
	}

	void init() {
		spi.disable();
		spi.configure();
		spi.set_tx_message_size(1); // not needed?
		spi.enable();
	}

	template<PacketType MessageType>
	void transmit_blocking(uint8_t byte) {
		spi.disable();
		spi.clear_EOT_flag();
		spi.clear_TXTF_flag();
		spi.set_tx_message_size(1);
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

	void config_bdma_transfer(uint32_t src, uint32_t sz) {
		uint32_t dst = spi.get_tx_datareg_addr();
		bdma.config_transfer(dst, src, sz);
	}

	void start_bdma_transfer(std::function<void(void)> &&cb) {
		callback = std::move(cb);
		bdma.register_callback(callback);

		start_bdma_transfer();
	}

	void start_bdma_transfer() {
		spi.disable();
		bdma.start_transfer();

		uint32_t sz = bdma.get_transfer_size();
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

	void init_mdma(std::function<void(void)> &&cb) {
		callback = std::move(cb);
		mdma.registerCallback([&]() { callback(); });
	}

	void start_mdma_transfer(uint32_t src, uint32_t sz, std::function<void(void)> &&cb) {
		callback = std::move(cb);
		mdma.registerCallback([&]() { callback(); });
		start_mdma_transfer(src, sz);
	}

	void start_mdma_transfer(uint32_t src, uint32_t sz) {
		spi.disable();
		uint32_t dst = spi.get_tx_datareg_addr();

		mdma.config_transfer((void *)dst, (void *)src, sz);

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
		mdma.start_transfer();
	}

	void wait_until_ready() {
		while (!spi.is_tx_complete())
			;
	}

private:
	SpiPeriph<typename ConfT::ScreenSpiConf> spi;
	typename ConfT::DCPin dcpin;
	std::function<void(void)> callback;

	MemoryTransfer mdma;
	target::BDMATransfer<typename ConfT::BDMAConf> bdma;

	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_te_flag_index;
};
