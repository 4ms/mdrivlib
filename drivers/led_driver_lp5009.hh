#pragma once
#include "drivers/i2c.hh"
#include "lp5009_registers.hh"
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>

namespace mdrivlib::LP5009
{

// LP5009 address (7-bit unshifted) = 0b00101aa, where aa is address pins
enum class Chip { LP5009, LP5024 };

template<enum Chip chip = Chip::LP5009>
struct Device {

	static constexpr unsigned NumRgbs = chip == Chip::LP5009 ? 3 : 4;

	mdrivlib::I2CPeriph &i2c;
	uint8_t dev_addr;

	Device(mdrivlib::I2CPeriph &i2c, uint8_t device_addr)
		: i2c{i2c}
		, dev_addr{device_addr} {
		i2c.enable_IT(3, 3);
	}

	bool init() {
		bool ok = write<DeviceConfig0>({.ChipEnable = 1});
		if (!ok)
			return false;

		ok = write<DeviceConfig1>({.GlobalOff = 0,
								   .MaxCurrentOption = 0,
								   .PWMDitheringEnable = 1,
								   .AutoIncEnable = 1,
								   .PowerSaveEnable = 0,
								   .LogScaleEnable = 1});
		if (!ok)
			return false;

		ok = write<LEDConfig>({.Led0BankEnable = 0, .Led1BankEnable = 0, .Led2BankEnable = 0, .Led3BankEnable = 0});
		if (!ok)
			return false;

		ok = all_leds_off();
		if (!ok)
			return false;

		uint32_t start = HAL_GetTick();
		while (!i2c.is_ready()) {
			if (HAL_GetTick() - start > 2000) // 2 seconds
				return false;
		}

		return !i2c.had_error();
	}

	void set_led_element(unsigned led_element_id, uint8_t value) {
		write_one(Out0Brightness::Address + led_element_id, value);
	}

	void set_rgb_led(unsigned rgb_led_id, const std::span<const uint8_t, 3> color) {
		write(Out0Brightness::Address + rgb_led_id * 3, color);
	}

	bool set_all_leds(const std::span<const uint8_t, NumRgbs * 3> values) {
		return write(Out0Brightness::Address, values);
	}

	void set_rgb_led_brightness(unsigned rgb_led_id, uint8_t brightness) {
		if (rgb_led_id < NumRgbs)
			write_one(LED0Brightness::Address + rgb_led_id, brightness);
	}

	void set_rgb_led_brightness(unsigned rgb_led_id, float brightness) {
		auto b = uint8_t(std::clamp(brightness, 0.f, 1.f) * 255.f);
		set_rgb_led_brightness(rgb_led_id, b);
	}

	void set_all_led_brightness(const std::span<const uint8_t, NumRgbs> brightnesses) {
		write(LED0Brightness::Address, brightnesses);
	}

	void dim_all_leds(uint8_t brightness) {
		for (unsigned i = 0; i < NumRgbs; i++)
			write_one(LED0Brightness::Address + i, brightness);
	}

	bool all_leds_off() {
		std::ranges::fill(_data, 0);
		return set_all_leds(_data);
	}

private:
	// Always copy to an internal buffer because lifetime must extend to end of I2C transfer
	// since we use interrrupts
	std::array<uint8_t, NumRgbs * 3> _data{};

	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c.write_reg(dev_addr, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	bool write_one(uint16_t mem_address, uint8_t data) {
		if (!i2c.is_ready())
			return false;
		_data[0] = data;
		return i2c.mem_write_IT(dev_addr, mem_address, I2C_MEMADD_SIZE_8BIT, &_data[0], 1) ==
			   mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	bool write(uint16_t mem_address, const std::span<const uint8_t> data) {
		if (!i2c.is_ready() || data.size() > _data.size())
			return false;

		if (_data.data() != data.data())
			std::ranges::copy(data, _data.begin());

		return i2c.mem_write_IT(dev_addr, mem_address, I2C_MEMADD_SIZE_8BIT, _data.data(), data.size()) ==
			   mdrivlib::I2CPeriph::I2C_NO_ERR;
	}
};

} // namespace mdrivlib::LP5009
