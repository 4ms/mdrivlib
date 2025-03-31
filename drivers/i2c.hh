#pragma once
#include "drivers/bus_register.hh"
#include "drivers/i2c_arch.hh"
#include "drivers/i2c_config_struct.hh"
#include "drivers/interrupt.hh"
#include <cstdint>
#include <optional>

namespace mdrivlib
{

class I2CPeriph {
public:
	enum Error { I2C_NO_ERR, I2C_INIT_ERR, I2C_ALREADY_INIT, I2C_XMIT_ERR };

	I2CPeriph() = default;
	~I2CPeriph() = default;

	I2CPeriph(const I2CConfig &defs) {
		init(defs);
	}
	Error init(const I2CConfig &defs);
	void deinit();

	bool is_ready();

	template<typename Reg>
	requires std::derived_from<Reg, BusReg::WriteAccess>
	Error write_reg(uint16_t dev_address, Reg data) {
		auto d = static_cast<uint8_t>(data);
		return mem_write(dev_address, Reg::Address, 1, &d, 1);
	}

	template<typename Reg>
	requires std::derived_from<Reg, BusReg::ReadAccess>
	std::optional<Reg> read_reg(uint16_t dev_address) {
		uint8_t data = 0;
		auto err = mem_read(dev_address, Reg::Address, 1, &data, 1);
		// TODO: return std::expected<Reg> when compilers support it
		if (err == I2C_NO_ERR)
			return Reg::make(data);
		return std::nullopt;
	}

	Error mem_read(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error mem_read_IT(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);

	Error mem_write(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error mem_write_IT(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error mem_write_dma(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);

	Error read(uint16_t dev_address, uint8_t *data, uint16_t size);
	Error read_IT(uint16_t dev_address, uint8_t *data, uint16_t size);

	Error write(uint16_t address, uint8_t *data, uint16_t size);
	Error write_IT(uint16_t address, uint8_t *data, uint16_t size);

	void enable_IT(uint8_t pri1 = 2, uint8_t pri2 = 2);
	void disable_IT();

#ifndef RK3566
	void link_DMA_TX(DMA_HandleTypeDef *dmatx);
	void link_DMA_RX(DMA_HandleTypeDef *dmarx);
#endif

	void enable_FMP();

private:
	bool already_init = false;
	I2C_Hal hal_i2c_;
	IRQn_Type i2c_irq_num_;
	IRQn_Type i2c_err_irq_num_;
	Interrupt event_isr;
	Interrupt error_isr;
	bool analog_filter;
	I2CConfig::DigitalFilterLevel digital_filter;

	void i2c_error_handler();
	void i2c_event_handler();

	uint32_t _check_errors(uint32_t retries);
};
} // namespace mdrivlib
