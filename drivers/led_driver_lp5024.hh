#pragma once
#include "drivers/i2c.hh"
#include "lp5024_registers.hh"
#include <cstdint>
#include <span>

namespace mdrivlib::LP5024
{

struct Device {

	mdrivlib::I2CPeriph &i2c;
	uint8_t dev_addr;

	Device(mdrivlib::I2CPeriph &i2c, uint8_t device_addr)
		: i2c{i2c}
		, dev_addr{device_addr} {
	}

	bool init() {
		bool ok = write<DeviceConfig0>({.ChipEnable = 1});
		if (!ok)
			return false;

		write<DeviceConfig1>({.GlobalOff = 0,
							  .MaxCurrentOption = 0,
							  .PWMDitheringEnable = 1,
							  .AutoIncEnable = 1,
							  .PowerSaveEnable = 0,
							  .LogScaleEnable = 1});

		write<LEDConfig>({.LedBankEnables = 0});

		all_leds_off();
		return true;
	}

	void set_led_element(unsigned led_element_id, uint8_t value) {
		write(Out0Brightness::Address + led_element_id, value);
	}

	void set_rgb_led(unsigned rgb_led_id, const std::span<const uint8_t, 3> color) {
		write(Out0Brightness::Address + rgb_led_id * 3, color);
	}

	void set_all_leds(const std::span<const uint8_t, 24> values) {
		write(Out0Brightness::Address, values);
	}

	void all_leds_off() {
		uint8_t data[24]{};
		set_all_leds(data);
	}

private:
	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c.write_reg(dev_addr, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	bool write(uint16_t mem_address, uint8_t data) {
		return i2c.mem_write(dev_addr, mem_address, I2C_MEMADD_SIZE_8BIT, &data, 1);
	}

	bool write(uint16_t mem_address, const std::span<const uint8_t> data) {
		return i2c.mem_write(
			dev_addr, mem_address, I2C_MEMADD_SIZE_8BIT, const_cast<uint8_t *>(data.data()), data.size());
	}

	template<typename Reg>
	Reg read() {
		static bool got_error_once = false;
		auto reg = i2c.read_reg<Reg>(dev_addr);

		if (!reg.has_value() && !got_error_once) {
			got_error_once = true;
		}
		return reg.value_or(Reg::make(0));
	}
};

} // namespace mdrivlib::LP5024
