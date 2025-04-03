#include "drivers/i2c.hh"
#include "drivers/cru_clksel.hh"
#include "drivers/cru_gate.hh"
#include "drivers/cru_reset.hh"
#include "drivers/i2c_target.hh"
#include <cstdint>
#include <cstdio>
#include <utility>

namespace mdrivlib
{

// Return the number of times left to retry
// Return 0 if we should stop retrying
uint32_t I2CPeriph::_check_errors(uint32_t retries) {
	printf("Retries left: %u, IPD = %08x\n", retries, hal_i2c_.instance->IPD);

	if (retries == 0)
		return 0;

	else if (error == I2C_TIMEOUT)
		return --retries;

	else if (error == I2C_NO_ERR)
		return 0;

	else
		return 0;
}

I2CPeriph::Error I2CPeriph::read(uint16_t dev_address, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::read_IT(uint16_t dev_address, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::write(uint16_t dev_address, uint8_t *data, uint16_t size) {
	if (size > 0)
		// Note if size == 1, then data param is ignored by mem_write
		return mem_write(dev_address, data[0], I2C_MEMADD_SIZE_8BIT, data + 1, size - 1);
	else
		return Error::I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::write_IT(uint16_t dev_address, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_read(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_read_IT(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

I2CPeriph::Error
I2CPeriph::mem_write(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	// std::array<uint32_t, 8> txdata{0};
	uint32_t txdata = 0;
	int pos = 0;

	auto append_data = [&txdata, &pos, this, size](uint8_t byte) {
		txdata |= byte << ((pos % 4) * 8);
		printf("%d: txdata[%d] |= %02x << %d\n", pos, pos / 4, byte, (pos % 4) * 8);
	};

	// Transmit
	using enum RockchipPeriph::I2C::EventBits;
	uint32_t retries = 16;
	while (retries) {

		if (!hal_i2c_.instance->perform_start()) {
			retries = _check_errors(retries);
			continue;
		}

		// 1st: Device address:
		append_data(dev_address);

		// 2nd: Register (memory) address (8 or 16-bit):
		append_data((uint8_t)mem_address);
		if (memadd_size == I2C_MEMADD_SIZE_16BIT)
			append_data((uint8_t)(mem_address >> 8));

		// 3rd: data to write to register (memory)
		while (size) {
			uint32_t fifo_remaining = std::min<unsigned>(size, 32);
			size -= fifo_remaining;

			while (fifo_remaining--) {
				append_data(*data++);

				pos++;
				if (pos % 4 == 0) {
					hal_i2c_.instance->TXDATA[pos / 4] = txdata;
					txdata = 0;
				}
			}

			hal_i2c_.instance->tx_mode();

			if (!hal_i2c_.instance->transmit(pos)) {
				retries = _check_errors(retries);
				continue;
			}
		}

		hal_i2c_.instance.disable();

		if (!hal_i2c_.instance->perform_stop()) {
			retries = _check_errors(retries);
			continue;
		}

		return I2C_NO_ERR;
	}

	printf("XMIT ERR\n");
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::mem_write_IT(
	uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

I2CPeriph::Error I2CPeriph::mem_write_dma(
	uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size) {
	return I2C_XMIT_ERR;
}

bool I2CPeriph::is_ready() {
	auto pending = hal_i2c_.instance->IPD & 0xFF;
	// TODO: need to check individual bits?
	return (pending);
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

	CruClksel::clk_i2c_sel::write(CruClksel::clk_i2c_clock_mux::clk_gpll_div_100m);
	CruGate::clk_i2c_en::write(CruGate::clock_enable);

	if (defs.I2C_periph_num == 1) {
		CruGate::clk_i2c1_en::write(CruGate::clock_enable);
		CruGate::pclk_i2c1_en::write(CruGate::clock_enable);

		Cru::resetn_i2c1::set();
		Cru::presetn_i2c1::set();
		Cru::presetn_i2c1::clear();
		Cru::resetn_i2c1::clear();

		hal_i2c_.instance = I2C1;

	} else if (defs.I2C_periph_num == 2) {
		CruGate::clk_i2c2_en::write(CruGate::clock_enable);
		CruGate::pclk_i2c2_en::write(CruGate::clock_enable);

		Cru::resetn_i2c2::set();
		Cru::presetn_i2c2::set();
		Cru::presetn_i2c2::clear();
		Cru::resetn_i2c2::clear();

		hal_i2c_.instance = I2C2;

	} else if (defs.I2C_periph_num == 3) {
		CruGate::clk_i2c3_en::write(CruGate::clock_enable);
		CruGate::pclk_i2c3_en::write(CruGate::clock_enable);

		Cru::resetn_i2c3::set();
		Cru::presetn_i2c3::set();
		Cru::presetn_i2c3::clear();
		Cru::resetn_i2c3::clear();

		hal_i2c_.instance = I2C3;

	} else if (defs.I2C_periph_num == 4) {
		CruGate::clk_i2c4_en::write(CruGate::clock_enable);
		CruGate::pclk_i2c4_en::write(CruGate::clock_enable);

		Cru::resetn_i2c4::set();
		Cru::presetn_i2c4::set();
		Cru::presetn_i2c4::clear();
		Cru::resetn_i2c4::clear();

		hal_i2c_.instance = I2C4;

	} else if (defs.I2C_periph_num == 5) {
		CruGate::clk_i2c5_en::write(CruGate::clock_enable);
		CruGate::pclk_i2c5_en::write(CruGate::clock_enable);

		// TODO:
		Cru::resetn_i2c5::set();
		Cru::presetn_i2c5::set();
		Cru::presetn_i2c5::clear();
		Cru::resetn_i2c5::clear();

		hal_i2c_.instance = I2C5;

	} else {
		// periph 0 is reserved for PMIC
		return I2C_INIT_ERR;
	}

	i2c_irq_num_ = 78 + defs.I2C_periph_num;

	hal_i2c_.instance->enable();

	// TODO: defs.timing.speed_hz => clock_div
	hal_i2c_.instance->clock_div(100, 100);

	// TODO: try these settings
	hal_i2c_.instance->enable_edge_glitch_filter();
	hal_i2c_.instance->h0_check_scl();
	hal_i2c_.instance->nack_release_scl_high();
	hal_i2c_.instance->set_slave_hold_scl_threshold(2);
	hal_i2c_.instance->filter_rising_edges(2);
	hal_i2c_.instance->filter_falling_edges(2);

	already_init = true;
	return I2C_NO_ERR;
}

void I2CPeriph::enable_IT(uint8_t pri1, uint8_t pri2) {
	event_isr.register_and_start_isr(i2c_irq_num_, pri1, pri2, [this]() { i2c_event_handler(); });
}

void I2CPeriph::disable_IT() {
	InterruptControl::disable_irq(i2c_irq_num_);
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
