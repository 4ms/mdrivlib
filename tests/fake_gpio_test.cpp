// clang-format off
#include "doctest.h"
#include "fake_gpio.h"
#include "drivers/pin.hh"
// clang-format on

TEST_CASE("fake_gpio_tests, fake_pin_init_at_0") {
	bool initval = read_fake_pin(FakeGPIO::B, 1);
	CHECK_FALSE(initval);
}

TEST_CASE("fake_gpio_tests, fake_pin_reads_what_was_written") {
	set_fake_pin(FakeGPIO::B, 1, true);
	bool postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK(postval);

	set_fake_pin(FakeGPIO::B, 1, false);
	postval = read_fake_pin(FakeGPIO::B, 1);
	CHECK_FALSE(postval);
}
