#pragma once
#include "adc_builtin_driver.hh"
#include "util/filter.hh"

template<AdcPeriphNum p, AdcChanNum c, int kOverSampleAmt = 128>
struct AnalogIn : AdcChan<p, c> {
	AnalogIn(GPIO port, uint8_t pin_num)
	{
		Pin pin(port, pin_num, PinMode::Analog);
	}

	void read()
	{
		oversampler_.add_val(this->get_val());
	}

	auto get()
	{
		return oversampler_.val();
	}

private:
	Oversampler<kOverSampleAmt, uint32_t> oversampler_;
};

template<AdcPeriphNum p, AdcChanNum c, typename FILTER>
struct AnalogInput : AdcChan<p, c> {
	AnalogInput(GPIO port, uint8_t pin_num)
	{
		Pin pin(port, pin_num, PinMode::Analog);
	}

	void read()
	{
		oversampler_.add_val(this->get_val());
	}

	auto get()
	{
		return oversampler_.val();
	}

private:
	FILTER oversampler_;
};

template<typename ADCIN, typename FILTER>
struct AnalogIn2 {
	AnalogIn2(GPIO port, uint8_t pin_num)
		: adcval(ADCIN::get_val_ptr())
	{
		Pin pin(port, pin_num, PinMode::Analog);
	}

	void read()
	{
		oversampler_.add_val(*adcval);
	}

	auto get()
	{
		return oversampler_.val();
	}

private:
	uint16_t *const adcval;
	FILTER oversampler_;
};

