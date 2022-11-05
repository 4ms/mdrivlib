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

template<unsigned FirstBitNum, unsigned BitWidth = 1, typename T>
constexpr T Bits(T raw) {
	return (raw >> FirstBitNum) & Mask<T>(BitWidth);
}

static_assert(Bits<0>(0b00011101) == 0b1);
static_assert(Bits<1>(0b00011101) == 0b0);
static_assert(Bits<2>(0b00011101) == 0b1);
static_assert(Bits<3>(0b00011101) == 0b1);
static_assert(Bits<4>(0b00011101) == 0b1);
static_assert(Bits<5>(0b00011101) == 0b0);
static_assert(Bits<6>(0b00011101) == 0b0);
static_assert(Bits<7>(0b00011101) == 0b0);
static_assert(Bits<0, 2>(0b00011101) == 0b01);
static_assert(Bits<1, 2>(0b00011101) == 0b10);
static_assert(Bits<2, 2>(0b00011101) == 0b11);
static_assert(Bits<0, 3>(0b00011101) == 0b101);
static_assert(Bits<1, 3>(0b00011101) == 0b110);
static_assert(Bits<2, 3>(0b00011101) == 0b111);
} // namespace BusReg
