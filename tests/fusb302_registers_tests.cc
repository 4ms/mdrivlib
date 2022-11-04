#include "doctest.h"
#include "fusb302_registers.hh"

TEST_CASE("Field order preserved, read back whats written") {
	FUSB302::InterruptA a{0x00};
	a.HardResetRx = 1;
	CHECK((uint8_t)a == 1);

	a.HardResetRx = 0;
	CHECK((uint8_t)a == 0);

	// Bit at LSB end
	constexpr FUSB302::InterruptA b{0x01};
	static_assert(b.HardResetRx == 1);
	CHECK(b.HardResetRx == 1);

	// Bit at MSB end
	constexpr FUSB302::InterruptA c{0x80};
	static_assert(c.OCPTempEvent == 1);
	CHECK(c.OCPTempEvent == 1);

	// Width 2 at the lsb end of struct
	constexpr FUSB302::Status0 s3{0b10101011};
	static_assert(s3.BCLevel == 3);
	CHECK(s3.BCLevel == 3);
	constexpr FUSB302::Status0 s2{0b10101010};
	static_assert(s2.BCLevel == 2);
	CHECK(s2.BCLevel == 2);
	constexpr FUSB302::Status0 s1{0b10101001};
	static_assert(s1.BCLevel == 1);
	CHECK(s1.BCLevel == 1);
	constexpr FUSB302::Status0 s0{0b10101000};
	static_assert(s0.BCLevel == 0);
	CHECK(s0.BCLevel == 0);

	// Width 2 in the middle of struct
	constexpr FUSB302::Control3 c3{0b00000110};
	static_assert(c3.NumRetries == 0b11);
	CHECK(c3.NumRetries == 0b11);
	constexpr FUSB302::Control3 c2{0b00000100};
	static_assert(c2.NumRetries == 0b10);
	CHECK(c2.NumRetries == 0b10);
	constexpr FUSB302::Control3 c1{0b00000010};
	static_assert(c1.NumRetries == 0b01);
	CHECK(c1.NumRetries == 0b01);
	constexpr FUSB302::Control3 c0{0b00000000};
	static_assert(c0.NumRetries == 0b00);
	CHECK(c0.NumRetries == 0b00);
}
