#pragma once
#include "drivers/dma_config.hh"
#include "drivers/pin.hh"

namespace mdrivlib
{
struct SaiConfig {
	uint32_t sai_periphnum;
	// SAI_TypeDef *sai;

	uint32_t tx_block_num;
	// SAI_Block_TypeDef *tx_block;

	uint32_t rx_block_num;
	// SAI_Block_TypeDef *rx_block;

	enum SAIRxTxMode { RXMaster, TXMaster, ExtSynced } mode;

	DMA_Config dma_init_tx;
	DMA_Config dma_init_rx;

	uint32_t datasize;
	uint32_t framesize;
	uint32_t samplerate;

	PinDef MCLK;
	PinDef SCLK;
	PinDef LRCLK;
	PinDef SD_DAC;
	PinDef SD_ADC;
	PinDef reset_pin;

	uint32_t bus_address;

	uint32_t num_tdm_ins;
	uint32_t num_tdm_outs;

	enum SyncSendMode { NoSendSync = 0b00, BlockASendsSync = 0b01, BlockBSendsSync = 0b11 } sync_send;

	enum SyncReceiveFrom {
		NoReceiveSync = -1,
		SyncToSAI1 = 0,
		SyncToSAI2 = 1,
		SyncToSAI3 = 2,
		SyncToSAI4 = 3
	} sync_receive_from;
};
} // namespace mdrivlib
