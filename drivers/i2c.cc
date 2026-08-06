#include "drivers/i2c.hh"
#include "drivers/clocks.hh"
#include "drivers/i2c_target.hh"
#include "drivers/interrupt.hh"
#include "drivers/rcc.hh"
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

constexpr uint32_t _I2C_FLAG_TIMEOUT = 1;
constexpr uint32_t _I2C_PROBE_TIMEOUT = 3;
constexpr uint32_t _I2C_LONG_TIMEOUT = 30;
constexpr uint32_t _I2C_VLONG_TIMEOUT = 100;

uint32_t I2CPeriph::_check_errors(uint32_t retries) {
	if (retries == 0)
		return 0;
	if (HAL_I2C_GetError(&hal_i2c_) != HAL_I2C_ERROR_AF)
		return 0;
	return --retries;
}

I2CPeriph::Error I2CPeriph::read(uint16_t dev_address, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Master_Receive(&hal_i2c_, dev_address, data, size, _I2C_LONG_TIMEOUT) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::read_IT(uint16_t dev_address, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Master_Receive_IT(&hal_i2c_, dev_address, data, size) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::write(uint16_t dev_address, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Master_Transmit(&hal_i2c_, dev_address, data, size, _I2C_LONG_TIMEOUT) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::write_IT(uint16_t dev_address, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Master_Transmit_IT(&hal_i2c_, dev_address, data, size) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_read(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Mem_Read(&hal_i2c_, dev_address, mem_address, memadd_size, data, size, _I2C_LONG_TIMEOUT) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_read_IT(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Mem_Read_IT(&hal_i2c_, dev_address, mem_address, memadd_size, data, size) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_write(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Mem_Write(&hal_i2c_, dev_address, mem_address, memadd_size, data, size, _I2C_LONG_TIMEOUT) ==
			HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::mem_write_IT(
	uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	clear_error();
	uint32_t retries = 16;
	while (retries) {
		if (HAL_I2C_Mem_Write_IT(&hal_i2c_, dev_address, mem_address, memadd_size, data, size) == HAL_OK)
			return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::mem_write_dma(
	uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	clear_error();
	HAL_StatusTypeDef err;
	while ((err = HAL_I2C_Mem_Write_DMA(&hal_i2c_, dev_address, mem_address, memadd_size, data, size)) != HAL_OK) {
		if (HAL_I2C_GetError(&hal_i2c_) != HAL_I2C_ERROR_AF)
			return I2C_XMIT_ERR;
	}
	return err == HAL_OK ? I2C_NO_ERR : I2C_XMIT_ERR;
}

bool I2CPeriph::is_ready() {
	return (HAL_I2C_GetState(&hal_i2c_) == HAL_I2C_STATE_READY);
}

bool I2CPeriph::clear_stuck_busy() {
	// Don't interrupt any transactions in flight
	if (HAL_I2C_GetState(&hal_i2c_) != HAL_I2C_STATE_READY)
		return false;

	if (__HAL_I2C_GET_FLAG(&hal_i2c_, I2C_FLAG_BUSY) == RESET)
		return false;

	__HAL_I2C_DISABLE(&hal_i2c_);

	// PE has to stay low for at least 3 APB cycles for the reset to take
	for (uint32_t i = 0; i < 16; i++)
		asm volatile("nop");

	__HAL_I2C_ENABLE(&hal_i2c_);

	return true;
}

bool I2CPeriph::probe(uint16_t dev_address) {
	clear_error();
	return HAL_I2C_IsDeviceReady(&hal_i2c_, dev_address, 1, _I2C_PROBE_TIMEOUT) == HAL_OK;
}

uint32_t I2CPeriph::get_error() const {
	return latched_error_;
}

bool I2CPeriph::had_error() const {
	return latched_error_ != HAL_I2C_ERROR_NONE;
}

void I2CPeriph::clear_error() {
	latched_error_ = HAL_I2C_ERROR_NONE;
}

// Store the HAL error code so we can detect a failed xfer that didn't
// hang or kill the bus
void I2CPeriph::latch_error() {
	if (hal_i2c_.ErrorCode != HAL_I2C_ERROR_NONE)
		latched_error_ |= hal_i2c_.ErrorCode;
}

void I2CPeriph::deinit() {
	// Clocks::I2C::disable(hal_i2c_.Instance);
	HAL_Delay(1);

	if (hal_i2c_.Instance == I2C1) {
		RCC_Reset::I2C1_::set();
		HAL_Delay(1);
		RCC_Reset::I2C1_::clear();
		HAL_Delay(1);
	} else if (hal_i2c_.Instance == I2C2) {
		RCC_Reset::I2C2_::set();
		HAL_Delay(1);
		RCC_Reset::I2C2_::clear();
		HAL_Delay(1);
	} else if (hal_i2c_.Instance == I2C3) {
		RCC_Reset::I2C3_::set();
		HAL_Delay(1);
		RCC_Reset::I2C3_::clear();
		HAL_Delay(1);
	} else if (hal_i2c_.Instance == I2C4) {
		RCC_Reset::I2C4_::set();
		HAL_Delay(1);
		RCC_Reset::I2C4_::clear();
		HAL_Delay(1);
	} else if (hal_i2c_.Instance == I2C5) {
		RCC_Reset::I2C5_::set();
		HAL_Delay(1);
		RCC_Reset::I2C5_::clear();
		HAL_Delay(1);
	} else if (hal_i2c_.Instance == I2C6) {
		RCC_Reset::I2C6_::set();
		HAL_Delay(1);
		RCC_Reset::I2C6_::clear();
		HAL_Delay(1);
	}

	already_init = false;
}

I2CPeriph::Error I2CPeriph::reset(const I2CConfig &defs) {
	// Mask the I2C interrupts before resetting so an in-flight
	// IRQ won't read the I2C registers when they're not clocked.
	disable_IT();

	deinit();
	clear_error();

	hal_i2c_.Lock = HAL_UNLOCKED;
	hal_i2c_.State = HAL_I2C_STATE_RESET;
	hal_i2c_.ErrorCode = HAL_I2C_ERROR_NONE;

	const auto err = init(defs);

	// A pending error IRQ unmasked here is harmless now that the handler only
	// latches: it reads flags that the re-init already cleared, and the next
	// transfer clears the latch before it starts.
	// TODO: if IT was not previously enabled, this would be wrong:
	enable_IT(defs.priority1, defs.priority2);
	return err;
}

I2CPeriph::Error I2CPeriph::init(const I2CConfig &defs) {
	Pin sda{defs.SDA.gpio,
			defs.SDA.pin,
			PinMode::Alt,
			defs.SDA.af,
			PinPull::Up,
			PinPolarity::Normal,
			PinSpeed::High,
			PinOType::OpenDrain};
	Pin scl{defs.SCL.gpio,
			defs.SCL.pin,
			PinMode::Alt,
			defs.SCL.af,
			PinPull::Up,
			PinPolarity::Normal,
			PinSpeed::High,
			PinOType::OpenDrain};

	// Call target-specific init:
	I2C::init(defs);

	// We let the pins be re-init because the app may have used them for some other purpose
	// But we don't re-init the I2C peripheral with each device on the bus
	if (already_init)
		return I2C_ALREADY_INIT;

	deinit();

	hal_i2c_.Instance = defs.I2Cx;
	hal_i2c_.Init.OwnAddress1 = 0x33;
	hal_i2c_.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hal_i2c_.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hal_i2c_.Init.OwnAddress2 = 0;
	hal_i2c_.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hal_i2c_.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
#ifdef STM32F4
	hal_i2c_.Init.ClockSpeed = defs.timing.speed_hz;
	hal_i2c_.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
	hal_i2c_.Init.Timing = defs.timing.calc();
	hal_i2c_.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
#endif

	if (hal_i2c_.Instance == I2C1) {
		i2c_irq_num_ = I2C1_EV_IRQn;
		i2c_err_irq_num_ = I2C1_ER_IRQn;
	} else if (hal_i2c_.Instance == I2C2) {
		i2c_irq_num_ = I2C2_EV_IRQn;
		i2c_err_irq_num_ = I2C2_ER_IRQn;
	} else if (hal_i2c_.Instance == I2C3) {
		i2c_irq_num_ = I2C3_EV_IRQn;
		i2c_err_irq_num_ = I2C3_ER_IRQn;
	} else if (hal_i2c_.Instance == I2C4) {
		i2c_irq_num_ = I2C4_EV_IRQn;
		i2c_err_irq_num_ = I2C4_ER_IRQn;
	} else if (hal_i2c_.Instance == I2C5) {
		i2c_irq_num_ = I2C5_EV_IRQn;
		i2c_err_irq_num_ = I2C5_ER_IRQn;
	} else if (hal_i2c_.Instance == I2C6) {
		i2c_irq_num_ = I2C6_EV_IRQn;
		i2c_err_irq_num_ = I2C6_ER_IRQn;
	}

	Clocks::I2C::enable(defs.I2Cx);

	HAL_I2C_DeInit(&hal_i2c_);
	if (HAL_I2C_Init(&hal_i2c_) != HAL_OK)
		return I2C_INIT_ERR;

	if (HAL_I2CEx_ConfigAnalogFilter(&hal_i2c_,
									 defs.analog_filter ? I2C_ANALOGFILTER_ENABLE : I2C_ANALOGFILTER_DISABLE) != HAL_OK)
		return I2C_INIT_ERR;

	if (HAL_I2CEx_ConfigDigitalFilter(&hal_i2c_, defs.digital_filter) != HAL_OK)
		return I2C_INIT_ERR;

	already_init = true;
	return I2C_NO_ERR;
}

void I2CPeriph::enable_IT(uint8_t pri1, uint8_t pri2) {
	event_isr.register_and_start_isr(i2c_irq_num_, pri1, pri2, [this]() { i2c_event_handler(); });
	error_isr.register_and_start_isr(i2c_err_irq_num_, pri1, pri2, [this]() { i2c_error_handler(); });
}

void I2CPeriph::disable_IT() {
	InterruptControl::disable_irq(i2c_irq_num_);
	InterruptControl::disable_irq(i2c_err_irq_num_);
}

void I2CPeriph::i2c_event_handler() {
	HAL_I2C_EV_IRQHandler(&hal_i2c_);
	// NACKs (HAL_I2C_ERROR_AF) are detected on the event path, not the error path
	latch_error();
}

void I2CPeriph::i2c_error_handler() {
	HAL_I2C_ER_IRQHandler(&hal_i2c_);
	latch_error();

	// Deliberately no HAL_I2C_Init() here. Re-initializing from interrupt
	// context reconfigures the peripheral underneath whatever HAL call is in
	// flight on the main loop, and forces State back to READY without clearing
	// Lock (it only does that when State is RESET, which it isn't mid-transfer)
	// -- leaving the handle half fixed-up and refusing every later transfer
	// before it reaches the bus.
	//
	// The error is latched instead, and the caller's state machine recovers via
	// reset(), which runs in the main context where re-init is safe.
}

void I2CPeriph::link_DMA_TX(DMA_HandleTypeDef *dmatx) {
	__HAL_LINKDMA(&hal_i2c_, hdmatx, *dmatx);
}

void I2CPeriph::link_DMA_RX(DMA_HandleTypeDef *dmarx) {
	__HAL_LINKDMA(&hal_i2c_, hdmarx, *dmarx);
}
} // namespace mdrivlib
