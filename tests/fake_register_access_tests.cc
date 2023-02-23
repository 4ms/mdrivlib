#include "doctest.h"
#include "stubs/drivers//register_access.hh"

TEST_CASE("Can Write and Read") {
	using TestReg1 = mdrivlib::RegisterBits<mdrivlib::ReadWrite, 123, 0xFFFFFFFF>;
	using TestReg2 = mdrivlib::RegisterBits<mdrivlib::ReadWrite, 456, 0xFFFFFFFF>;

	TestReg1::write(999);
	TestReg2::write(111);
	CHECK(TestReg1::read() == 999);
	CHECK(TestReg2::read() == 111);

	TestReg1::write(888);
	CHECK(TestReg1::read() == 888);
	CHECK(TestReg2::read() == 111);

	TestReg2::write(222);
	CHECK(TestReg1::read() == 888);
	CHECK(TestReg2::read() == 222);
};

TEST_CASE("Can Set and Clear") {
	using TestReg = mdrivlib::RegisterBits<mdrivlib::ReadWrite, 123, 0b11>;
	using TestRegBit0 = mdrivlib::RegisterBits<mdrivlib::ReadWrite, 123, (1 << 0)>;
	using TestRegBit1 = mdrivlib::RegisterBits<mdrivlib::ReadWrite, 123, (1 << 1)>;

	TestRegBit0::clear();
	TestRegBit1::clear();
	CHECK(TestReg::read() == 0b00);
	CHECK(TestRegBit0::read() == 0);
	CHECK(TestRegBit1::read() == 0);

	TestRegBit0::set();
	CHECK(TestReg::read() == 0b01);
	CHECK(TestRegBit0::read() == 1);
	CHECK(TestRegBit1::read() == 0);

	TestRegBit1::set();
	CHECK(TestReg::read() == 0b11);
	CHECK(TestRegBit0::read() == 1);
	CHECK(TestRegBit1::read() == 1);

	TestRegBit0::clear();
	CHECK(TestReg::read() == 0b10);
	CHECK(TestRegBit0::read() == 0);
	CHECK(TestRegBit1::read() == 1);

	TestRegBit1::clear();
	CHECK(TestReg::read() == 0b00);
	CHECK(TestRegBit0::read() == 0);
	CHECK(TestRegBit1::read() == 0);
};
