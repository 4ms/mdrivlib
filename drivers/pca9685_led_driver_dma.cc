#include "interrupt.hh"
#include "pca9685_led_driver.hh"

PCA9685DmaDriver::DMADriver::DMADriver(PCA9685DmaDriver &parent,
									   const DMAConfig &dma_defs,
									   PCA9685DmaDriver::FrameBuffer &frame_buf)
	: driver_(parent)
	, frame_buffer(frame_buf)
	, dma_defs_{dma_defs} {

	// if (dma_defs_.continuous)
	// 	HALCallback transfer_complete_cb{HALCallbackID::I2C_MemTxCplt, [this]() {
	// 										 advance_frame_buffer();
	// 										 write_current_frame_to_chip();
	// 									 }};
	// else
	// 	HALCallback transfer_complete_cb{HALCallbackID::I2C_MemTxCplt, [this]() {
	// 										 advance_frame_buffer();
	// 										 if (!dma_defs_.continuous && cur_chip_num_ == 0)
	// 											 transfer_complete = true;
	// 										 else
	// 											 write_current_frame_to_chip();
	// 									 }};
}

LEDDriverError PCA9685DmaDriver::DMADriver::start_dma() {
	frame_buffer_cur_pos = frame_buffer;

	auto err = init_dma();
	if (err != LEDDriverError::None)
		return err;

	// Link I2C and DMA
	driver_.i2c_periph_.link_DMA_TX(&hal_dma_h_);

	InterruptManager::registerISR(dma_defs_.IRQn, [dmah_ptr = &hal_dma_h_]() { HAL_DMA_IRQHandler(dmah_ptr); });

	HAL_NVIC_SetPriority(dma_defs_.IRQn, dma_defs_.pri, dma_defs_.subpri);
	HAL_NVIC_EnableIRQ(dma_defs_.IRQn);

	write_current_frame_to_chip();

	if (driver_.led_error_ == LEDDriverError::DMA_XMIT_ERR)
		return LEDDriverError::DMA_XMIT_ERR;
	return LEDDriverError::None;
}

LEDDriverError PCA9685DmaDriver::DMADriver::init_dma() {
	System::enable_dma_rcc(dma_defs_.DMAx);
	hal_dma_h_.Instance = dma_defs_.stream;

	// Todo: get DMA working on H7x5 chips
#if !defined(STM32H745xx) && !defined(STM32H755xx)
	hal_dma_h_.Init.Channel = dma_defs_.channel;
#endif
	hal_dma_h_.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hal_dma_h_.Init.PeriphInc = DMA_PINC_DISABLE;
	hal_dma_h_.Init.MemInc = DMA_MINC_ENABLE;
	hal_dma_h_.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hal_dma_h_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hal_dma_h_.Init.Mode = DMA_NORMAL; // DMA_CIRCULAR
	hal_dma_h_.Init.Priority = DMA_PRIORITY_LOW;
	hal_dma_h_.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hal_dma_h_.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hal_dma_h_.Init.MemBurst = DMA_MBURST_SINGLE;
	hal_dma_h_.Init.PeriphBurst = DMA_PBURST_SINGLE;

	HAL_DMA_DeInit(&hal_dma_h_);
	auto hal_err = HAL_DMA_Init(&hal_dma_h_);
	if (hal_err != HAL_OK)
		return LEDDriverError::DMA_XMIT_ERR;

	return LEDDriverError::None;
}

void PCA9685DmaDriver::DMADriver::advance_frame_buffer() {
	++cur_chip_num_;
	if (cur_chip_num_ >= driver_.num_chips_)
		cur_chip_num_ = 0;

	uint32_t start_of_cur_chip = cur_chip_num_ * PCA9685DmaDriver::kNumLedsPerChip;
	frame_buffer_cur_pos = &(frame_buffer[start_of_cur_chip]);
}

void PCA9685DmaDriver::DMADriver::write_current_frame_to_chip() {
	uint16_t chip_address = I2C_BASE_ADDRESS | (cur_chip_num_ << 1);
	auto err = driver_.i2c_periph_.mem_write_dma(chip_address,
												 REG_LED0,
												 REGISTER_ADDR_SIZE,
												 reinterpret_cast<uint8_t *>(frame_buffer_cur_pos),
												 PCA9685DmaDriver::kNumLedsPerChip * sizeof(frame_buffer[0]));

	if (err != I2CPeriph::Error::I2C_NO_ERR)
		driver_.led_error_ = LEDDriverError::DMA_XMIT_ERR;
}

