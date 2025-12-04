#pragma once
#include "drivers/i2c.hh"
#include "lp5009_registers.hh"
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

		write<DeviceConfig1>({.GlobalOff = 0,
							  .MaxCurrentOption = 0,
							  .PWMDitheringEnable = 1,
							  .AutoIncEnable = 1,
							  .PowerSaveEnable = 0,
							  .LogScaleEnable = 1});

		write<LEDConfig>({.Led0BankEnable = 0, .Led1BankEnable = 0, .Led2BankEnable = 0, .Led3BankEnable = 0});

		all_leds_off();
		return true;
	}

	void set_led_element(unsigned led_element_id, uint8_t value) {
		write(Out0Brightness::Address + led_element_id, value);
	}

	void set_rgb_led(unsigned rgb_led_id, const std::span<const uint8_t, 3> color) {
		write(Out0Brightness::Address + rgb_led_id * 3, color);
	}

	void set_all_leds(const std::span<const uint8_t, NumRgbs * 3> values) {
		write(Out0Brightness::Address, values);
	}

	void set_rgb_led_brightness(unsigned rgb_led_id, uint8_t brightness) {
		if (rgb_led_id < NumRgbs)
			write(LED0Brightness::Address + rgb_led_id, brightness);
	}

	void set_rgb_led_brightness(unsigned rgb_led_id, float brightness) {
		auto u8_brightness = std::clamp<uint8_t>(0, 255, brightness * 255.f);
		set_rgb_led_brightness(rgb_led_id, u8_brightness);
	}

	void set_all_led_brightness(const std::span<const uint8_t, NumRgbs> brightnesses) {
		write(LED0Brightness::Address, brightnesses);
	}

	void dim_all_leds(uint8_t brightness) {
		for (unsigned i = 0; i < NumRgbs; i++)
			write(LED0Brightness::Address + i, brightness);
	}

	void all_leds_off() {
		uint8_t data[NumRgbs * 3]{};
		set_all_leds(data);
	}

private:
	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c.write_reg(dev_addr, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	bool write(uint16_t mem_address, uint8_t data) {
		return i2c.mem_write_IT(dev_addr, mem_address, I2C_MEMADD_SIZE_8BIT, &data, 1);
	}

	bool write(uint16_t mem_address, const std::span<const uint8_t> data) {
		return i2c.mem_write_IT(
			dev_addr, mem_address, I2C_MEMADD_SIZE_8BIT, const_cast<uint8_t *>(data.data()), data.size());
	}
};

} // namespace mdrivlib::LP5009
