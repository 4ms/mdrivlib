#pragma once
#include <algorithm>
#include <cstdint>

namespace BusReg
{

// R/W access tag types:
struct ReadAccess {};
struct WriteAccess {};
struct ReadOnly : ReadAccess {};
struct WriteOnly : WriteAccess {};
struct ReadWrite : ReadAccess, WriteAccess {};
// Read/WriteClear are just a hint for the developer (for now)
struct WriteClear : WriteAccess {};
struct ReadClear : ReadAccess {};

template<typename T>
constexpr T Mask(T width) {
	return (T(1) << width) - T(1);
}
static_assert(Mask(1) == 0b1);
static_assert(Mask(2) == 0b11);
static_assert(Mask(3) == 0b111);
static_assert(Mask(4) == 0b1111);
static_assert(Mask(9) == 0x1FF);
static_assert(Mask<uint8_t>(9) == 0xFF);
static_assert(Mask<uint8_t>(10) == 0xFF);

// I thought about specifying the bits with <FirstBit, Width>
// but it makes checking for mistakes harder. E.g.:
//	.SomeField1 = Bits<2, 3> //OK: bits 2, 3, and 4
//	.SomeField2 = Bits<6> //oops, skipped bit 5
// But here's the replacement function in case we want it later:
//
// template<unsigned FirstBitNum, unsigned Width = 1, typename T>
// constexpr T Bits(T raw) {
// 	return (raw >> FirstBitNum) & Mask<T>(Width);
// }

template<unsigned LowestBitNum, unsigned... BitNum, typename T>
constexpr T Bits(T raw) {
	static_assert(std::max({LowestBitNum, BitNum...}) - std::min({LowestBitNum, BitNum...}) == sizeof...(BitNum),
				  "Bits must be consectutive");
	return (raw >> LowestBitNum) & Mask<T>(sizeof...(BitNum) + 1);
}

static_assert(Bits<0>(0b00011101) == 0b1);
static_assert(Bits<1>(0b00011101) == 0b0);
static_assert(Bits<2>(0b00011101) == 0b1);
static_assert(Bits<3>(0b00011101) == 0b1);
static_assert(Bits<4>(0b00011101) == 0b1);
static_assert(Bits<5>(0b00011101) == 0b0);
static_assert(Bits<6>(0b00011101) == 0b0);
static_assert(Bits<7>(0b00011101) == 0b0);
static_assert(Bits<0, 1>(0b00011101) == 0b01);
static_assert(Bits<1, 2>(0b00011101) == 0b10);
static_assert(Bits<2, 3>(0b00011101) == 0b11);
static_assert(Bits<0, 1, 2>(0b00011101) == 0b101);
static_assert(Bits<1, 2, 3>(0b00011101) == 0b110);
static_assert(Bits<2, 3, 4>(0b00011101) == 0b111);

// Should fail: cannot use non-consectutive Bits such as 3, 4, 7:
// static_assert(Bitz<3, 4, 7>(0) == 0);

} // namespace BusReg
