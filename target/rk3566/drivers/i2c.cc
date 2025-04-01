#include "drivers/i2c.hh"
#include "drivers/cru_clksel.hh"
#include "drivers/cru_gate.hh"
#include "drivers/cru_reset.hh"
#include "drivers/i2c_target.hh"
#include "drivers/interrupt.hh"
#include "drivers/register_access.hh"
#include <cstdint>
#include <utility>

namespace mdrivlib
{

constexpr uint32_t _I2C_FLAG_TIMEOUT = 1;
constexpr uint32_t _I2C_LONG_TIMEOUT = 30;
constexpr uint32_t _I2C_VLONG_TIMEOUT = 100;

uint32_t I2CPeriph::_check_errors(uint32_t retries) {
	if (retries == 0)
		return 0;
	if (true)
		return 0;
	return --retries;
}

I2CPeriph::Error I2CPeriph::read(uint16_t dev_address, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		// if (HAL_I2C_Master_Receive(&hal_i2c_, dev_address, data, size, _I2C_LONG_TIMEOUT) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::read_IT(uint16_t dev_address, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		// if (HAL_I2C_Master_Receive_IT(&hal_i2c_, dev_address, data, size) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

static void transmit(volatile RockchipPeriph::I2C *instance, unsigned size) {
	instance->MTXCNT = size;

	auto finished_bit = std::to_underlying(RockchipPeriph::I2C::IPD_bits::mtxcnt_tx_finished);
	while ((instance->IPD & finished_bit) == 0) {
		instance->IPD = finished_bit;
	}
}

I2CPeriph::Error I2CPeriph::write(uint16_t dev_address, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		hal_i2c_.instance->tx_mode();
		hal_i2c_.instance->send_start();

		int word_cnt = 0;
		int filled = 0;

		while (size >= 4) {
			uint32_t word = *data++;
			word |= (*data++ << 8);
			word |= (*data++ << 16);
			word |= (*data++ << 24);
			hal_i2c_.instance->TXDATA[word_cnt] = word;
			word_cnt++;
			filled += 4;
			size -= 4;

			if (word_cnt == 8) {
				transmit(hal_i2c_.instance, 32);
				word_cnt = 0;
				filled = 0;
			}
		}

		if (size > 0) {
			uint32_t word = 0;
			word |= *data++;
			if (size > 1)
				word |= (*data++ << 8);
			if (size > 2)
				word |= (*data++ << 16);
			filled += size;
			hal_i2c_.instance->TXDATA[word_cnt] = word;
		}

		if (filled) {
			transmit(hal_i2c_.instance, filled);
		}

		hal_i2c_.instance->send_stop();

		// if (HAL_I2C_Master_Transmit(&hal_i2c_, dev_address, data, size, _I2C_LONG_TIMEOUT) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::write_IT(uint16_t dev_address, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {

		// if (HAL_I2C_Master_Transmit_IT(&hal_i2c_, dev_address, data, size) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_read(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		// if (HAL_I2C_Mem_Read(&hal_i2c_, dev_address, mem_address, memadd_size, data, size, _I2C_LONG_TIMEOUT) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_read_IT(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		// if (HAL_I2C_Mem_Read_IT(&hal_i2c_, dev_address, mem_address, memadd_size, data, size) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_write(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		// if (HAL_I2C_Mem_Write(&hal_i2c_, dev_address, mem_address, memadd_size, data, size, _I2C_LONG_TIMEOUT) ==
		// 	HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::mem_write_IT(
	uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	uint32_t retries = 16;
	while (retries) {
		// if (HAL_I2C_Mem_Write_IT(&hal_i2c_, dev_address, mem_address, memadd_size, data, size) == HAL_OK)
		// 	return I2C_NO_ERR;
		retries = _check_errors(retries);
	}
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::mem_write_dma(
	uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	// HAL_StatusTypeDef err;
	// while ((err = HAL_I2C_Mem_Write_DMA(&hal_i2c_, dev_address, mem_address, memadd_size, data, size)) != HAL_OK) {
	// if (HAL_I2C_GetError(&hal_i2c_) != HAL_I2C_ERROR_AF)
	// 	return I2C_XMIT_ERR;
	// }
	// return err == HAL_OK ? I2C_NO_ERR : I2C_XMIT_ERR;
	return I2C_XMIT_ERR;
}

bool I2CPeriph::is_ready() {
	// return (HAL_I2C_GetState(&hal_i2c_) == HAL_I2C_STATE_READY);
	return false;
}

void I2CPeriph::deinit() {
	// Clocks::I2C::disable(hal_i2c_.Instance);
	// Clocks::I2C::force_reset(hal_i2c_.Instance);
	already_init = false;
}

I2CPeriph::Error I2CPeriph::init(const I2CConfig &defs) {
	using namespace RockchipPeriph;

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
	I2CTarget::init(defs);

	// We let the pins be re-init because the app may have used them for some other purpose
	// But we don't re-init the I2C peripheral with each device on the bus
	if (already_init)
		return I2C_ALREADY_INIT;

	deinit();

	if (defs.I2C_periph_num == 2) {
		CruClksel::clk_i2c_sel::write(CruClksel::clk_i2c_clock_mux::clk_gpll_div_100m);
		CruGate::clk_i2c_en::write(CruGate::clock_enable);
		CruGate::clk_i2c2_en::write(CruGate::clock_enable);
		CruGate::pclk_i2c2_en::write(CruGate::clock_enable);

		Cru::resetn_i2c2::set();
		Cru::presetn_i2c2::set();
		Cru::presetn_i2c2::clear();
		Cru::resetn_i2c2::clear();

		hal_i2c_.instance = I2C2;

	} else {
		return I2C_INIT_ERR;
	}

	hal_i2c_.instance->enable();
	hal_i2c_.instance->clock_div(100, 100);
	// hal_i2c_.Instance = PeriphUtil::I2C(defs.I2C_periph_num);
	// hal_i2c_.Init.OwnAddress1 = 0x33;
	// hal_i2c_.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	// hal_i2c_.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	// hal_i2c_.Init.OwnAddress2 = 0;
	// hal_i2c_.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	// hal_i2c_.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	// #ifdef STM32F4
	// hal_i2c_.Init.ClockSpeed = defs.timing.speed_hz;
	// hal_i2c_.Init.DutyCycle = I2C_DUTYCYCLE_2;
	// #else
	// hal_i2c_.Init.Timing = defs.timing.calc();
	// hal_i2c_.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	// #endif

	// if (hal_i2c_.Instance == I2C1) {
	// 	i2c_irq_num_ = I2C1_EV_IRQn;
	// 	i2c_err_irq_num_ = I2C1_ER_IRQn;
	// } else if (hal_i2c_.Instance == I2C2) {
	// 	i2c_irq_num_ = I2C2_EV_IRQn;
	// 	i2c_err_irq_num_ = I2C2_ER_IRQn;
	// } else if (hal_i2c_.Instance == I2C3) {
	// 	i2c_irq_num_ = I2C3_EV_IRQn;
	// 	i2c_err_irq_num_ = I2C3_ER_IRQn;
	// } else if (hal_i2c_.Instance == I2C4) {
	// 	i2c_irq_num_ = I2C4_EV_IRQn;
	// 	i2c_err_irq_num_ = I2C4_ER_IRQn;
	// } else if (hal_i2c_.Instance == I2C5) {
	// 	i2c_irq_num_ = I2C5_EV_IRQn;
	// 	i2c_err_irq_num_ = I2C5_ER_IRQn;
	// } else if (hal_i2c_.Instance == I2C6) {
	// 	i2c_irq_num_ = I2C6_EV_IRQn;
	// 	i2c_err_irq_num_ = I2C6_ER_IRQn;
	// }

	// Clocks::I2C::enable(defs.I2Cx);

	// HAL_I2C_DeInit(&hal_i2c_);
	// if (HAL_I2C_Init(&hal_i2c_) != HAL_OK)
	// 	return I2C_INIT_ERR;

	// if (HAL_I2CEx_ConfigAnalogFilter(&hal_i2c_,
	// 								 defs.analog_filter ? I2C_ANALOGFILTER_ENABLE : I2C_ANALOGFILTER_DISABLE) != HAL_OK)
	// 	return I2C_INIT_ERR;

	// if (HAL_I2CEx_ConfigDigitalFilter(&hal_i2c_, defs.digital_filter) != HAL_OK)
	// 	return I2C_INIT_ERR;

	// already_init = true;
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
	// HAL_I2C_EV_IRQHandler(&hal_i2c_);
}

void I2CPeriph::i2c_error_handler() {
	// HAL_I2C_ER_IRQHandler(&hal_i2c_);
	// if (hal_i2c_.ErrorCode != HAL_I2C_ERROR_NONE) {
	// 	HAL_I2C_Init(&hal_i2c_);
	// }
}

} // namespace mdrivlib
