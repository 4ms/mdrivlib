#pragma once
#include <cstdint>

namespace mdrivlib::IRQ
{

enum : uint32_t {
	GPIO0_IRQ = 65,
	GPIO1_IRQ = 66,
	GPIO2_IRQ = 67,
	GPIO3_IRQ = 68,
	GPIO4_IRQ = 69,

	I2C0_IRQ = 78,
	I2C1_IRQ = 79,
	I2C2_IRQ = 80,
	I2C3_IRQ = 81,
	I2C4_IRQ = 82,
	I2C5_IRQ = 83,

	I2S0_8CH_IRQ = 84,
	I2S1_8CH_IRQ = 85,
	I2S2_2CH_IRQ = 86,
	I2S3_2CH_IRQ = 87,

	PWM0_IRQ = 114,
	PWM1_IRQ = 115,
	PWM2_IRQ = 116,
	PWM3_IRQ = 117,
};

} // namespace mdrivlib::IRQ
