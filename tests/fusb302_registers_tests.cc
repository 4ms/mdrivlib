#include "doctest.h"
#include "drivers/fusb302_registers.hh"
#include <cstring>

template<typename Reg>
Reg read_reg(uint8_t fake_val) {
	(void)Reg::Address;
	return Reg::make(fake_val);
}

template<typename Reg>
void write_reg(Reg data) {
	(void)Reg::Address;
	auto d = static_cast<uint8_t>(data);
	(void)d; //fake write it
}

TEST_CASE("Simple API to read a reg and use field names on data that's read") {
	auto a = read_reg<FUSB302::InterruptA>(0x00);
	CHECK((a.HardResetRx == 0));

	FUSB302::InterruptA a2{read_reg<FUSB302::InterruptA>(0x01)};
	if (a2.HardResetRx == 1)
		CHECK(true);
	else
		CHECK(false);
}

TEST_CASE("Convert to uint8_t") {
	FUSB302::InterruptA a;
	std::memset(&a, 0, sizeof(a));
	a.HardResetRx = 1;
	CHECK((uint8_t)a == 1);
	a.SoftResetRx = 1;
	CHECK((uint8_t)a == 0b11);
	a.HardResetRx = 0;
	CHECK((uint8_t)a == 0b10);
}

struct RegisterInit {
	uint8_t reg_num;
	uint8_t value;
};
TEST_CASE("Syntax supports storing reg value in an array") {
	using namespace FUSB302;
	RegisterInit r[] = {
		{InterruptA::Address, InterruptA{.HardResetRx = 0}},
		{InterruptB::Address, InterruptB{.GCRSent = 0}},
	};
	(void)r;
}

TEST_CASE("Can use write_reg<RegType>({.NamedDesignator1=1, .NamedDesignator2=1,...})") {
	write_reg<FUSB302::InterruptA>({.HardResetRx = 1});
	write_reg<FUSB302::InterruptA>({.HardResetRx = 1, .SoftResetRx = 1});
}

TEST_CASE("constexpr and bit endianness") {
	// Bit at LSB end
	constexpr auto b = FUSB302::InterruptA::make(0x01);
	static_assert(b.HardResetRx == 1);
	CHECK(b.HardResetRx == 1);

	// Bit at MSB end
	constexpr auto c = FUSB302::InterruptA::make(0x80);
	static_assert(c.OCPTempEvent == 1);
	CHECK(c.OCPTempEvent == 1);

	// Width 2 at the lsb end of struct
	constexpr auto s3 = FUSB302::Status0::make(0b10101011);
	static_assert(s3.BCLevel == 3);
	CHECK(s3.BCLevel == 3);
	constexpr auto s2 = FUSB302::Status0::make(0b10101010);
	static_assert(s2.BCLevel == 2);
	CHECK(s2.BCLevel == 2);
	constexpr auto s1 = FUSB302::Status0::make(0b10101001);
	static_assert(s1.BCLevel == 1);
	CHECK(s1.BCLevel == 1);
	constexpr auto s0 = FUSB302::Status0::make(0b10101000);
	static_assert(s0.BCLevel == 0);
	CHECK(s0.BCLevel == 0);

	// Width 2 in the middle of struct
	constexpr auto c3 = FUSB302::Control3::make(0b00000110);
	static_assert(c3.NumRetries == 0b11);
	CHECK(c3.NumRetries == 0b11);
	constexpr auto c2 = FUSB302::Control3::make(0b00000100);
	static_assert(c2.NumRetries == 0b10);
	CHECK(c2.NumRetries == 0b10);
	constexpr auto c1 = FUSB302::Control3::make(0b00000010);
	static_assert(c1.NumRetries == 0b01);
	CHECK(c1.NumRetries == 0b01);
	constexpr auto c0 = FUSB302::Control3::make(0b00000000);
	static_assert(c0.NumRetries == 0b00);
	CHECK(c0.NumRetries == 0b00);
}
