#pragma once

#include "conf/led_driver_conf.hh"
#include "dma_config_struct.hh"
#include "hal_callback.hh"
#include "i2c.hh"
#include "led_driver.hh"
#include "stm32xx.h"

// Todo: the DMa drivers inherits from non-dma driver? Or else remove all non-DMA commands from DMA drivr
class PCA9685DmaDriver : public ILedDmaDriver {
public:
	const static int kNumLedsPerChip = 16;
	const static int kNumRGBLEDsPerChip = 5;
	const static size_t kRequiredBufferSize = kNumLedDriverChips * PCA9685DmaDriver::kNumLedsPerChip;

	using FrameBuffer = uint32_t[kRequiredBufferSize];

public:
	PCA9685DmaDriver(I2CPeriph &i2c, uint32_t num_chips, const DMA_Config &dma_defs, FrameBuffer &frame_buf);

	virtual LEDDriverError start();
	virtual void start_it_mode();
	virtual LEDDriverError start_dma_mode();
	virtual uint32_t *const get_buf_addr(const uint32_t chip_num, const uint32_t led_num);

	LEDDriverError set_rgb_led(uint8_t led_number, uint16_t c_red, uint16_t c_green, uint16_t c_blue);
	LEDDriverError set_single_led(uint8_t led_element_number, uint16_t brightness);
	uint8_t get_red_led_element_id(uint8_t rgb_led_id);
	uint8_t get_chip_num(uint8_t rgb_led_id);

private:
	I2CPeriph &i2c_periph_;
	uint32_t num_chips_;
	LEDDriverError led_error_;

	LEDDriverError write_register(uint8_t driverAddr, uint8_t registerAddr, uint8_t registerValue);
	LEDDriverError reset_chip(uint8_t driverAddr);
	LEDDriverError write_once(uint16_t chip_num);

	// Todo: pull this out of PCA driver, it should be a template class so we can use other LedDmaDrivers
	class DMADriver {
		friend class PCA9685DmaDriver;

	public:
		DMADriver(PCA9685DmaDriver &parent, const DMA_Config &dma_defs, FrameBuffer &frame_buf);
		LEDDriverError start_dma();

	private:
		LEDDriverError init_dma();
		void advance_frame_buffer();
		void write_current_frame_to_chip();

		PCA9685DmaDriver &driver_;
		DMA_HandleTypeDef hal_dma_h_;
		Interrupt dma_task;
		uint8_t cur_chip_num_ = 0;
		uint32_t *frame_buffer_cur_pos;
		uint32_t (&frame_buffer)[kRequiredBufferSize];
		const DMA_Config &dma_defs_;
		bool transfer_complete;
		// Todo: Fix HALCallbacks so we can move a temporary (stack-created) lambda into HALCBs
		HALCallback transfer_complete_cb{HALCallbackID::I2C_MemTxCplt, [this]() {
											 advance_frame_buffer();
											 if (!dma_defs_.continuous && cur_chip_num_ == 0)
												 transfer_complete = true;
											 else
												 write_current_frame_to_chip();
										 }};
	};
	friend class PCA9685DmaDriver::DMADriver;
	DMADriver dma_;

private:
	const static inline uint32_t REG_MODE1 = 0x00;
	const static inline uint32_t REG_MODE2 = 0x01;
	const static inline uint32_t REG_LED0 = 0x06;
	const static inline uint32_t I2C_BASE_ADDRESS = 0b10000000;
	const static inline auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};

class PCA9685Driver : public ILedDmaDriver {
	const static int kNumLedsPerChip = 16;
	const static int kNumRGBLEDsPerChip = 5;
	const static size_t kRequiredBufferSize = kNumLedDriverChips * PCA9685DmaDriver::kNumLedsPerChip;

	using FrameBuffer = uint32_t[kRequiredBufferSize];

public:
	PCA9685Driver(I2CPeriph &i2c, uint32_t num_chips, FrameBuffer &frame_buf);

	virtual LEDDriverError start();
	virtual void start_it_mode();
	virtual LEDDriverError start_dma_mode();
	virtual uint32_t *const get_buf_addr(const uint32_t chip_num, const uint32_t led_num);

	LEDDriverError set_rgb_led(uint8_t led_number, uint16_t c_red, uint16_t c_green, uint16_t c_blue);
	LEDDriverError set_single_led(uint8_t led_element_number, uint16_t brightness);
	uint8_t get_red_led_element_id(uint8_t rgb_led_id);
	uint8_t get_chip_num(uint8_t rgb_led_id);

private:
	I2CPeriph &i2c_periph_;
	uint32_t num_chips_;
	LEDDriverError led_error_;

	LEDDriverError write_register(uint8_t driverAddr, uint8_t registerAddr, uint8_t registerValue);
	LEDDriverError reset_chip(uint8_t driverAddr);

	const static inline uint32_t REG_MODE1 = 0x00;
	const static inline uint32_t REG_MODE2 = 0x01;
	const static inline uint32_t REG_LED0 = 0x06;
	const static inline uint32_t I2C_BASE_ADDRESS = 0b10000000;
	const static inline auto REGISTER_ADDR_SIZE = I2C_MEMADD_SIZE_8BIT;
};

