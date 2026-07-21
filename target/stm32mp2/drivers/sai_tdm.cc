#include "drivers/sai_tdm.hh"
#include "drivers/cache.hh"
#include "drivers/clocks.hh"
#include "drivers/hpdma_transfer.hh"
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

SaiTdmPeriph::Error SaiTdmPeriph::change_samplerate_blocksize(uint32_t sample_rate, uint32_t block_size) {
	saidef_.samplerate = sample_rate;

	tx_block_size_ = block_size * (saidef_.num_tdm_outs * 2);
	rx_block_size_ = block_size * (saidef_.num_tdm_ins * 2);

	if (init() == Error::SAI_NO_ERR) {
		return Error::SAI_NO_ERR;
	} else
		return Error::SAI_INIT_ERR;
}

SaiTdmPeriph::Error SaiTdmPeriph::init() {
	_init_pins();
	_enable_clocks();

	Error err = SAI_NO_ERR;

	// Todo: swap order: always init slave first
	err = _config_tx_sai();
	if (err != SAI_NO_ERR)
		return err;

	err = _config_rx_sai();
	if (err != SAI_NO_ERR)
		return err;

	tx_irqn = saidef_.dma_init_tx.IRQn;
	InterruptControl::set_irq_priority(tx_irqn, saidef_.dma_init_tx.pri, saidef_.dma_init_tx.subpri);
	InterruptControl::disable_irq(tx_irqn);

	rx_irqn = saidef_.dma_init_rx.IRQn;
	InterruptControl::set_irq_priority(rx_irqn, saidef_.dma_init_rx.pri, saidef_.dma_init_rx.subpri);
	InterruptControl::disable_irq(rx_irqn);

	_sai_enable(&hsai_rx);
	_sai_enable(&hsai_tx);

	return SAI_NO_ERR;
}

void SaiTdmPeriph::_enable_clocks() {
	// Kernel clock (flexgen XBAR channel) must be configured by the app -- see rcc_xbar.hh
	Clocks::SAI::reset(saidef_.sai);
	Clocks::SAI::enable(saidef_.sai);

	auto enable_hpdma = [](const DMA_TypeDef *DMAx) {
		if (DMAx == nullptr)
			return;
		else if (DMAx == HPDMA1)
			RCC_Enable::HPDMA1_::set();
		else if (DMAx == HPDMA2)
			RCC_Enable::HPDMA2_::set();
		else if (DMAx == HPDMA3)
			RCC_Enable::HPDMA3_::set();
	};
	enable_hpdma(saidef_.dma_init_rx.DMAx);
	enable_hpdma(saidef_.dma_init_tx.DMAx);
}

SaiTdmPeriph::Error SaiTdmPeriph::_config_rx_sai() {
	Error err = SAI_NO_ERR;

	__HAL_SAI_RESET_HANDLE_STATE(&hsai_rx);
	hsai_rx.Instance = saidef_.rx_block;

	_sai_disable(&hsai_rx);

	if (saidef_.mode == SaiConfig::RXMaster) {
		hsai_rx.Init.AudioMode = SAI_MODEMASTER_RX;
		hsai_rx.Init.Synchro = SAI_ASYNCHRONOUS;
		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
		hsai_rx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::TXMaster) {
		hsai_rx.Init.AudioMode = SAI_MODESLAVE_RX;
		hsai_rx.Init.Synchro = SAI_SYNCHRONOUS;
		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_rx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::ExtSynced) {
		hsai_rx.Init.AudioMode = SAI_MODESLAVE_RX;
		hsai_rx.Init.Synchro = saidef_.sync_receive_from == SaiConfig::SyncToSAI1 ? SAI_SYNCHRONOUS_EXT_SAI1
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI2 ? SAI_SYNCHRONOUS_EXT_SAI2
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI3 ? SAI_SYNCHRONOUS_EXT_SAI3
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI4 ? SAI_SYNCHRONOUS_EXT_SAI4
																				  : SAI_SYNCHRONOUS;

		hsai_rx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_rx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	}

	hsai_rx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_rx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_rx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_rx.Init.AudioFrequency = saidef_.samplerate;
	hsai_rx.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_rx.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_rx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	hsai_rx.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_rx.Init.PdmInit.Activation = DISABLE;

	if (saidef_.num_tdm_ins > 2) {
		// Todo: use conf to set slot size
		// Todo: allow conf to set FS pulse mode (ActiveFrameLength)
		hsai_rx.Init.Protocol = SAI_FREE_PROTOCOL;
		hsai_rx.Init.DataSize = saidef_.datasize;
		hsai_rx.Init.FirstBit = SAI_FIRSTBIT_MSB;
		hsai_rx.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
		hsai_rx.FrameInit.FrameLength = saidef_.framesize;
		hsai_rx.FrameInit.ActiveFrameLength = 1; // FS pulses at start of frame
		hsai_rx.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
		hsai_rx.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
		hsai_rx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
		hsai_rx.SlotInit.FirstBitOffset = 0;
		hsai_rx.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
		hsai_rx.SlotInit.SlotNumber = saidef_.num_tdm_ins;
		hsai_rx.SlotInit.SlotActive = 0x0000FFFF;
		HAL_SAI_DeInit(&hsai_rx);
		if (HAL_SAI_Init(&hsai_rx) != HAL_OK)
			err = SAI_INIT_ERR;
	} else {
		HAL_SAI_DeInit(&hsai_rx);
		if (HAL_SAI_InitProtocol(&hsai_rx, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
			err = SAI_INIT_ERR;
	}

	return err;
}

SaiTdmPeriph::Error SaiTdmPeriph::_config_tx_sai() {
	Error err = SAI_NO_ERR;

	__HAL_SAI_RESET_HANDLE_STATE(&hsai_tx);
	hsai_tx.Instance = saidef_.tx_block;
	_sai_disable(&hsai_tx);

	if (saidef_.mode == SaiConfig::RXMaster) {
		hsai_tx.Init.AudioMode = SAI_MODESLAVE_TX;
		hsai_tx.Init.Synchro = SAI_SYNCHRONOUS;
		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_tx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::TXMaster) {
		hsai_tx.Init.AudioMode = SAI_MODEMASTER_TX;
		hsai_tx.Init.Synchro = SAI_ASYNCHRONOUS;
		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
		hsai_tx.Init.SynchroExt = saidef_.sync_send == SaiConfig::BlockASendsSync ? SAI_SYNCEXT_OUTBLOCKA_ENABLE
								: saidef_.sync_send == SaiConfig::BlockBSendsSync ? SAI_SYNCEXT_OUTBLOCKB_ENABLE
																				  : SAI_SYNCEXT_DISABLE;
	}
	if (saidef_.mode == SaiConfig::ExtSynced) {
		hsai_tx.Init.AudioMode = SAI_MODESLAVE_TX;
		hsai_tx.Init.Synchro = saidef_.sync_receive_from == SaiConfig::SyncToSAI1 ? SAI_SYNCHRONOUS_EXT_SAI1
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI2 ? SAI_SYNCHRONOUS_EXT_SAI2
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI3 ? SAI_SYNCHRONOUS_EXT_SAI3
							 : saidef_.sync_receive_from == SaiConfig::SyncToSAI4 ? SAI_SYNCHRONOUS_EXT_SAI4
																				  : SAI_SYNCHRONOUS;

		hsai_tx.Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;
		hsai_tx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	}

	hsai_tx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	hsai_tx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	hsai_tx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
	hsai_tx.Init.AudioFrequency = saidef_.samplerate;
	hsai_tx.Init.MonoStereoMode = SAI_STEREOMODE;
	hsai_tx.Init.CompandingMode = SAI_NOCOMPANDING;
	hsai_tx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	hsai_tx.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
	hsai_tx.Init.PdmInit.Activation = DISABLE;

	if (saidef_.num_tdm_outs > 2) {
		hsai_tx.Init.Protocol = SAI_FREE_PROTOCOL;
		hsai_tx.Init.DataSize = saidef_.datasize;
		hsai_tx.Init.FirstBit = SAI_FIRSTBIT_MSB;
		hsai_tx.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
		hsai_tx.FrameInit.FrameLength = saidef_.framesize;
		hsai_tx.FrameInit.ActiveFrameLength = 1; // FS pulses at start of frame
		hsai_tx.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
		hsai_tx.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
		hsai_tx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
		hsai_tx.SlotInit.FirstBitOffset = 0;
		hsai_tx.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
		hsai_tx.SlotInit.SlotNumber = saidef_.num_tdm_outs;
		hsai_tx.SlotInit.SlotActive = 0x0000FFFF;
		HAL_SAI_DeInit(&hsai_tx);
		if (HAL_SAI_Init(&hsai_tx) != HAL_OK)
			err = SAI_INIT_ERR;
	} else {
		HAL_SAI_DeInit(&hsai_tx);
		if (HAL_SAI_InitProtocol(&hsai_tx, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
			err = SAI_INIT_ERR;
	}

	return err;
}

SaiTdmPeriph::Error SaiTdmPeriph::_config_tx_dma() {
	hdma_tx.Instance = saidef_.dma_init_tx.stream;
	hdma_tx.Init.Request = saidef_.dma_init_tx.channel;
	hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;

	DMA_NodeConfTypeDef node_conf;
	node_conf.SrcAddress = (uintptr_t)tx_buf_ptr_;
	node_conf.DstAddress = reinterpret_cast<uintptr_t>(&saidef_.tx_block->DR);
	node_conf.DataSize = tx_block_size_ * sizeof(uint32_t); // block sizes are in 32-bit samples

	if (!HPDmaCircular::setup(hdma_tx, dma_tx_queue, dma_tx_node, node_conf))
		return SAI_INIT_ERR;

	__HAL_LINKDMA(&hsai_tx, hdmatx, hdma_tx);
	return SAI_NO_ERR;
}

SaiTdmPeriph::Error SaiTdmPeriph::_config_rx_dma() {
	hdma_rx.Instance = saidef_.dma_init_rx.stream;
	hdma_rx.Init.Request = saidef_.dma_init_rx.channel;
	hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;

	DMA_NodeConfTypeDef node_conf;
	node_conf.SrcAddress = reinterpret_cast<uintptr_t>(&saidef_.rx_block->DR);
	node_conf.DstAddress = (uintptr_t)rx_buf_ptr_;
	node_conf.DataSize = rx_block_size_ * sizeof(uint32_t);

	if (!HPDmaCircular::setup(hdma_rx, dma_rx_queue, dma_rx_node, node_conf))
		return SAI_INIT_ERR;

	__HAL_LINKDMA(&hsai_rx, hdmarx, hdma_rx);
	return SAI_NO_ERR;
}

void SaiTdmPeriph::_init_pins() {
	if (saidef_.mode == SaiConfig::TXMaster || saidef_.mode == SaiConfig::RXMaster) {
		Pin sai_mclk{saidef_.MCLK.gpio,
					 saidef_.MCLK.pin,
					 PinMode::Alt,
					 saidef_.MCLK.af,
					 PinPull::None,
					 PinPolarity::Normal,
					 PinSpeed::High,
					 PinOType::PushPull};
		Pin sai_sclk{saidef_.SCLK.gpio,
					 saidef_.SCLK.pin,
					 PinMode::Alt,
					 saidef_.SCLK.af,
					 PinPull::None,
					 PinPolarity::Normal,
					 PinSpeed::High,
					 PinOType::PushPull};
		Pin sai_lrclk{saidef_.LRCLK.gpio,
					  saidef_.LRCLK.pin,
					  PinMode::Alt,
					  saidef_.LRCLK.af,
					  PinPull::None,
					  PinPolarity::Normal,
					  PinSpeed::High,
					  PinOType::PushPull};
	}
	Pin sai_mrx_adc{saidef_.SD_ADC.gpio,
					saidef_.SD_ADC.pin,
					PinMode::Alt,
					saidef_.SD_ADC.af,
					PinPull::None,
					PinPolarity::Normal,
					PinSpeed::High,
					PinOType::PushPull};
	Pin sai_mtx_dac{saidef_.SD_DAC.gpio,
					saidef_.SD_DAC.pin,
					PinMode::Alt,
					saidef_.SD_DAC.af,
					PinPull::None,
					PinPolarity::Normal,
					PinSpeed::High,
					PinOType::PushPull};
}

void SaiTdmPeriph::set_tx_buffer(uint8_t *tx_buf_ptr, uint32_t frames_per_block) {
	tx_buf_ptr_ = tx_buf_ptr;
	tx_block_size_ = frames_per_block * saidef_.num_tdm_outs * 2;
}

void SaiTdmPeriph::set_rx_buffer(uint8_t *rx_buf_ptr, uint32_t frames_per_block) {
	rx_buf_ptr_ = rx_buf_ptr;
	rx_block_size_ = frames_per_block * saidef_.num_tdm_ins * 2;
}

void SaiTdmPeriph::set_tx_buffer_start(uint8_t *tx_buf_ptr, uint32_t block_size) {
	tx_buf_ptr_ = tx_buf_ptr;
	tx_block_size_ = block_size;
}

void SaiTdmPeriph::set_rx_buffer_start(uint8_t *rx_buf_ptr, uint32_t block_size) {
	rx_buf_ptr_ = rx_buf_ptr;
	rx_block_size_ = block_size;
}

void SaiTdmPeriph::set_callbacks(Interrupt::ISRType &&tx_complete_cb, Interrupt::ISRType &&tx_half_complete_cb) {
	tx_tc_cb = std::move(tx_complete_cb);
	tx_ht_cb = std::move(tx_half_complete_cb);
}

void SaiTdmPeriph::start() {
	// DMA linked-list setup happens here (not in init()) because the nodes contain
	// the buffer addresses.
	if (_config_tx_dma() != SAI_NO_ERR)
		return;
	if (_config_rx_dma() != SAI_NO_ERR)
		return;

	if (saidef_.mode == SaiConfig::RXMaster)
		_start_irq(rx_irqn, &hdma_rx);
	if (saidef_.mode == SaiConfig::TXMaster)
		_start_irq(tx_irqn, &hdma_tx);
	if (saidef_.mode == SaiConfig::ExtSynced)
		stop(); // disable interrupts for synced SAI

	fe_errors = 0;
	te_errors = 0;
	dme_errors = 0;

	// Note: in linked-list mode the HAL re-writes the head node's size/addresses
	// from these args, so they must match what _config_*_dma() put in the nodes.
	HAL_SAI_Transmit_DMA(&hsai_tx, tx_buf_ptr_, tx_block_size_);
	HAL_SAI_Receive_DMA(&hsai_rx, rx_buf_ptr_, rx_block_size_);

	// The HAL updated the node contents (in cached memory) after they were
	// cleaned; clean again so the DMA's link fetches see the same values.
	SystemCache::clean_dcache_by_range(&dma_tx_node, sizeof(dma_tx_node));
	SystemCache::clean_dcache_by_range(&dma_tx_queue, sizeof(dma_tx_queue));
	SystemCache::clean_dcache_by_range(&dma_rx_node, sizeof(dma_rx_node));
	SystemCache::clean_dcache_by_range(&dma_rx_queue, sizeof(dma_rx_queue));
}

void SaiTdmPeriph::_start_irq(IRQn_Type irqn, DMA_HandleTypeDef *hdma) {
	InterruptManager::register_isr(irqn, [this, hdma]() {
		if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_HT) != 0U) {
			__HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_HT);
			tx_ht_cb();
		}

		if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_TC) != 0U) {
			__HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TC);
			tx_tc_cb();
		}

		if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_DTE) != 0U) {
			__HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_DTE);
			te_errors++;
		}
		if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_USE) != 0U) {
			__HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_USE);
			fe_errors++;
		}
		if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_ULE) != 0U) {
			__HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_ULE);
			dme_errors++;
		}
	});
	InterruptControl::enable_irq(irqn);
}

void SaiTdmPeriph::stop() {
	InterruptControl::disable_irq(tx_irqn);
	InterruptControl::disable_irq(rx_irqn);
	HAL_SAI_Abort(&hsai_tx);
	HAL_SAI_Abort(&hsai_rx);
}

void SaiTdmPeriph::pause() {
	if (saidef_.mode == SaiConfig::RXMaster)
		InterruptControl::disable_irq(rx_irqn);

	else if (saidef_.mode == SaiConfig::TXMaster)
		InterruptControl::disable_irq(tx_irqn);
}

void SaiTdmPeriph::resume() {
	if (saidef_.mode == SaiConfig::RXMaster)
		InterruptControl::enable_irq(rx_irqn);

	else if (saidef_.mode == SaiConfig::TXMaster)
		InterruptControl::enable_irq(tx_irqn);
}

void SaiTdmPeriph::_sai_enable(SAI_HandleTypeDef *hsai) {
	hsai->Instance->CR1 = hsai->Instance->CR1 | (0x1UL << 16U);
}

void SaiTdmPeriph::_sai_disable(SAI_HandleTypeDef *hsai) {
	hsai->Instance->CR1 = hsai->Instance->CR1 & ~(0x1UL << 16U);
}

} // namespace mdrivlib
