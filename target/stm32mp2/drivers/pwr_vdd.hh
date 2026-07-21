#pragma once
// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
#include "drivers/stm32xx.h"

namespace mdrivlib
{

namespace PowerControl
{

// RM0457, Section 19.4.9:
// The independent supplies (VDDIO1, VDDIO2,VDDIO3, VDDIO4, VDD33USB, VDD33UCPD and VDDA18ADC)
// are not considered as present by default, and a logical and electrical isolation is
// applied to ignore any information coming from the peripherals supplied by these dedicated
// supplies.
// • If these supplies are shorted externally to VDD, the application must assume they are
//   	available without enabling any peripheral voltage monitoring. The power isolation can
//   	be removed by setting the corresponding supply valid bits.
// • If these supplies are independent from VDD, the PVM can be enabled to confirm
//   whether the supply is present or not.

// VDDIO1: PE[5:0]
// VDDIO2: PE[15:6]
// VDDIO3: PD[11:0]
// VDDIO4: PB[11:0]
// VDDA18ADC: ADC pins
// VDD33UCPD: USB-C CC1 and CC2 pins

enum class Present { Always, If };

// PE[5:0]
inline void enable_vddio1(Present present)
{
	if (present == Present::If) {
		PWR->CR8 |= PWR_CR8_VDDIO1VMEN;
		while (!(PWR->CR8 & PWR_CR8_VDDIO1RDY))
			;
		PWR->CR8 &= ~PWR_CR8_VDDIO1VMEN;
	}

	PWR->CR8 |= PWR_CR8_VDDIO1SV;
}

// PE[15:6]
inline void enable_vddio2(Present present)
{
	if (present == Present::If) {
		PWR->CR7 |= PWR_CR7_VDDIO2VMEN;
		while (!(PWR->CR7 & PWR_CR7_VDDIO2RDY))
			;
		PWR->CR7 &= ~PWR_CR7_VDDIO2VMEN;
	}

	PWR->CR7 |= PWR_CR7_VDDIO2SV;
}

// PD[11:0]
inline void enable_vddio3(Present present)
{
	if (present == Present::If) {
		PWR->CR1 |= PWR_CR1_VDDIO3VMEN;
		while (!(PWR->CR1 & PWR_CR1_VDDIO3RDY))
			;
		PWR->CR1 &= ~PWR_CR1_VDDIO3VMEN;
	}

	PWR->CR1 |= PWR_CR1_VDDIO3SV;
}

// PB[11:0]
inline void enable_vddio4(Present present)
{
	if (present == Present::If) {
		PWR->CR1 |= PWR_CR1_VDDIO4VMEN;
		while (!(PWR->CR1 & PWR_CR1_VDDIO4RDY))
			;
		PWR->CR1 &= ~PWR_CR1_VDDIO4VMEN;
	}

	PWR->CR1 |= PWR_CR1_VDDIO4SV;
}

// ADC
inline void enable_vdda18adc(Present present)
{
	if (present == Present::If) {
		PWR->CR1 |= PWR_CR1_AVMEN;
		while (!(PWR->CR1 & PWR_CR1_ARDY))
			;
		PWR->CR1 &= ~PWR_CR1_AVMEN;
	}

	PWR->CR1 |= PWR_CR1_ASV;
}

// USB2 PHY (no SV bit; Always case is a no-op)
inline void enable_usb33(Present present)
{
	if (present == Present::If) {
		PWR->CR1 |= PWR_CR1_USB33VMEN;
		while (!(PWR->CR1 & PWR_CR1_USB33RDY))
			;
		PWR->CR1 &= ~PWR_CR1_USB33VMEN;
	}
}

// USB-C CC1/2 pins
inline void enable_usbcc(Present present)
{
	if (present == Present::If) {
		PWR->CR1 |= PWR_CR1_UCPDVMEN;
		while (!(PWR->CR1 & PWR_CR1_UCPDRDY))
			;
		PWR->CR1 &= ~PWR_CR1_UCPDVMEN;
	}

	PWR->CR1 |= PWR_CR1_UCPDSV;
}

} // namespace PowerControl

} // namespace mdrivlib
