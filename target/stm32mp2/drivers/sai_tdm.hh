#pragma once
#include "drivers/interrupt.hh"
#include "drivers/pin.hh"
#include "drivers/sai_config_struct.hh"

// SAI TDM driver for STM32MP2. Same public API as the stm32mp1 SaiTdmPeriph.
//
// Differences from MP1 (see target/stm32mp2/README.md):
// - DMA is the HPDMA, which has no hardware circular mode: each direction uses a
//   one-node circular linked-list queue (drivers/hpdma_transfer.hh), a pattern
//   verified in stm32mp2-baremetal hal-audio.
// - The SAI kernel clock (RCC flexgen XBAR channel, e.g. from a PLL) must be
//   configured by the app before calling init() -- see drivers/rcc_xbar.hh.
// - DMA_Config.stream is the HPDMA channel (e.g. HPDMA1_Channel0) and
//   DMA_Config.channel is the HPDMA request (e.g. HPDMA_REQUEST_SAI2_A).
//
// NOT YET VERIFIED ON HARDWARE: port of the verified stereo I2S flow to the MP1
// driver's TDM structure. Verify with a converted hal-audio baremetal example.

namespace mdrivlib
{
class SaiTdmPeriph {
public:
	enum Error {
		SAI_NO_ERR,
		SAI_INIT_ERR,
		SAI_I2C_ERR,
		SAI_XMIT_ERR,
	};

	SaiTdmPeriph(const SaiConfig &def)
		: saidef_(def) {
	}

	~SaiTdmPeriph() = default;

	Error init();
	void set_rx_buffer_start(uint8_t *rx_buf_ptr, uint32_t block_size);
	void set_tx_buffer_start(uint8_t *tx_buf_ptr, uint32_t block_size);
	void set_tx_buffer(uint8_t *tx_buf_ptr, uint32_t frames_per_block);
	void set_rx_buffer(uint8_t *tx_buf_ptr, uint32_t frames_per_block);
	void set_callbacks(Interrupt::ISRType &&tx_complete_cb, Interrupt::ISRType &&tx_half_complete_cb);

	void start();
	void stop();

	void pause();
	void resume();

	Error change_samplerate_blocksize(uint32_t sample_rate, uint32_t block_size);

	uint32_t te_errors = 0;	 // data transfer errors
	uint32_t fe_errors = 0;	 // user setting errors
	uint32_t dme_errors = 0; // update link errors

private:
	SaiConfig saidef_;
	DMA_HandleTypeDef hdma_tx;
	DMA_HandleTypeDef hdma_rx;
	alignas(64) DMA_NodeTypeDef dma_tx_node;
	alignas(64) DMA_QListTypeDef dma_tx_queue;
	alignas(64) DMA_NodeTypeDef dma_rx_node;
	alignas(64) DMA_QListTypeDef dma_rx_queue;
	SAI_HandleTypeDef hsai_tx;
	SAI_HandleTypeDef hsai_rx;
	IRQn_Type tx_irqn;
	IRQn_Type rx_irqn;
	uint8_t *tx_buf_ptr_;
	uint8_t *rx_buf_ptr_;
	uint32_t tx_block_size_;
	uint32_t rx_block_size_;

	void _init_pins();
	void _enable_clocks();
	Error _config_rx_sai();
	Error _config_tx_sai();
	Error _config_rx_dma();
	Error _config_tx_dma();
	void _sai_enable(SAI_HandleTypeDef *hsai);
	void _sai_disable(SAI_HandleTypeDef *hsai);
	void _start_irq(IRQn_Type irqn, DMA_HandleTypeDef *hdma);

	Interrupt::ISRType tx_tc_cb;
	Interrupt::ISRType tx_ht_cb;
};
} // namespace mdrivlib
