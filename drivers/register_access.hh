#pragma once
#include <cstdint>
#include <utility>

namespace mdrivlib
{
using regsize_t = uint32_t;

// Mask Helpers
// from github.com/kensmith/cortex-from-scratch
template<regsize_t width>
struct generate_unshifted_mask_t {
	enum { value = (generate_unshifted_mask_t<width - 1>::value << 1) | 1 };
};
template<>
struct generate_unshifted_mask_t<0> {
	enum { value = 0 };
};
template<regsize_t offset, regsize_t width>
struct generate_mask_t {
	enum { value = generate_unshifted_mask_t<width>::value << offset };
};
template<regsize_t offset, regsize_t width>
constexpr regsize_t mask_v = generate_mask_t<offset, width>::value;
static_assert(mask_v<2, 4> == 0b111100);

// first_bit():
// Returns the position of the first set bit (LSB = 0)
// Returns greater than the register size if no bits are set
constexpr regsize_t first_bit(regsize_t mask) {
	regsize_t fb = 0;
	if (mask == 0)
		return sizeof(regsize_t) * 8;
	while ((mask & 1) == 0) {
		mask >>= 1;
		fb++;
	}
	return fb;
}
static_assert(first_bit(0b00000001) == 0);
static_assert(first_bit(0b00000010) == 1);
static_assert(first_bit(0x80000000) == 31);

// RegisterBits<>: Bits in register specified by mask
template<typename AccessPolicyT, regsize_t address, regsize_t mask>
struct RegisterBits {
	static constexpr regsize_t BaseAddress = address;
	static constexpr regsize_t Mask = mask;
	static constexpr regsize_t offset = first_bit(mask);

	static regsize_t read() {
		return AccessPolicyT::read(reinterpret_cast<volatile regsize_t *>(address), offset, mask);
	}
	static void write(regsize_t val) {
		AccessPolicyT::write(reinterpret_cast<volatile regsize_t *>(address), offset, mask, val);
	}
	static void set() {
		AccessPolicyT::set(reinterpret_cast<volatile regsize_t *>(address), mask);
	}
	static void clear() {
		AccessPolicyT::clear(reinterpret_cast<volatile regsize_t *>(address), mask);
	}
};

// RegisterSection<>: Bits in register specified by offset and width
// Deprecated?
template<typename AccessPolicyT, regsize_t address, regsize_t offset, regsize_t width>
using RegisterSection = RegisterBits<AccessPolicyT, address, mask_v<offset, width>>;

// Null register (periph does not exist)
struct NonexistantRegister {
	static regsize_t read() {
		return 0;
	}
	static void write(regsize_t val) {
	}
	static void set() {
	}
	static void clear() {
	}
};

// Access Policies:

struct ReadOnly {
	static regsize_t read(volatile regsize_t *address, regsize_t offset, regsize_t mask) {
		return (*address & mask) >> offset;
	}
};

struct WriteOnly {
	static void write(volatile regsize_t *address, regsize_t offset, regsize_t mask, regsize_t val) {
		// printf("wr %p 0x%08x\n", address, (val << offset) & mask);
		*address = ((val << offset) & mask);
	}
	static void set(volatile regsize_t *address, regsize_t mask) {
		// printf("wr %p 0x%08x\n", address, (unsigned int)mask);
		*address = mask;
	}
};

struct ReadWrite : ReadOnly {
	static void write(volatile regsize_t *address, regsize_t offset, regsize_t mask, regsize_t val) {
		*address = (*address & ~mask) | ((val << offset) & mask);
	}
	static void set(volatile regsize_t *address, regsize_t mask) {
		*address = *address | mask;
	}
	static void clear(volatile regsize_t *address, regsize_t mask) {
		*address = *address & ~mask;
	}
};

// RegisterDualSetClear<>: set() sets the Set bit(s), clear() sets the Clear bit(s).
// Access to Set/Clear is always WriteOnly.
template<regsize_t SetAddress, regsize_t SetMask, regsize_t ClearAddress, regsize_t ClearMask>
struct RegisterDualSetClear {
	static void set() {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(SetAddress), SetMask);
	}
	static void clear() {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(ClearAddress), ClearMask);
	}
};

// RegisterSetClear<>: shortcut for RegisterDualSetClear by specifying types
template<typename SetT, typename ClearT>
using RegisterSetClear = RegisterDualSetClear<SetT::BaseAddress, SetT::Mask, ClearT::BaseAddress, ClearT::Mask>;

// Helpers for RegisterMasked16:
static constexpr uint32_t masked_set_bit(uint8_t bit) {
	return (1 << bit) | (1 << (bit + 16));
}

static constexpr uint32_t masked_clr_bit(uint8_t bit) {
	return (1 << (bit + 16));
}

static constexpr uint32_t masked(uint16_t mask, uint16_t bits) {
	return (mask << 16) | bits;
}

// Rockchip-style 32-bit register where:
// - Top 16 bits are a mask of which bits to set/clear in the register.
// - Bottom 16 bits are the actual bits to set and clear.
//
// e.g. 0xFF001234 (== 0b 1111'1111'0000'0000 ' 0001'0010'0011'0100)
// will set bits 12 and 9; clear bits 8, 10, 11, 13, 14, 15; (i.e. 0x1200)
// and not modify bits 0-7 (i.e. the 0x34 is ignored)
template<regsize_t Address, uint16_t Mask, uint16_t Shift>
struct RegisterMasked16 {
	static constexpr uint32_t ShiftedMask = Mask << Shift;

	static void set() {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(Address), masked(ShiftedMask, ShiftedMask));
	}
	static void write(uint16_t value) {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(Address), masked(ShiftedMask, value << Shift));
	}
	static void clear() {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(Address), masked(ShiftedMask, 0));
	}
};

template<regsize_t Address, uint16_t Mask, uint16_t Shift, typename ValuesT>
struct RegisterMaskedChoice {
	static constexpr uint32_t ShiftedMask = Mask << Shift;

	static void write(ValuesT value) {
		WriteOnly::set(reinterpret_cast<volatile regsize_t *>(Address),
					   masked(ShiftedMask, (std::to_underlying(value) << Shift) & ShiftedMask));
	}
};

template<regsize_t Address, uint16_t BitNum>
using RegisterMasked16BitNum = RegisterMasked16<Address, 0b1, BitNum>;

} // namespace mdrivlib
