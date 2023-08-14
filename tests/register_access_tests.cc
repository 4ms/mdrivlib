#include "doctest.h"
#include "drivers/register_access.hh"
#include <memory>

TEST_CASE("register accesss") {
	using namespace mdrivlib;

	// FIXME: how to get the address of a variable in a constexpr way?
	// int REG;
	// using X = RegisterBits<ReadWrite, std::addressof(REG), 0x000FFF00>;

	CHECK(0b1110000 == mask_v<4, 3>);
	CHECK(0b0001111 == mask_v<0, 4>);

	CHECK(first_bit(0xFFFFFFFF) == 0);
	CHECK(first_bit(0x7FFFFFFF) == 0);
	CHECK(first_bit(0b00111100) == 2);
	CHECK(first_bit(0b0011110) == 1);
	CHECK(first_bit(0b001111) == 0);
	CHECK(first_bit(0) == sizeof(unsigned long) * 8);
}
