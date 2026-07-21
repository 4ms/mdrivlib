#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace MMU
{

enum class MemType {
	Noncache = 0,
	Normal = 1,
	Device = 2,
};
constexpr uint64_t MemTypeShift = 2;

enum class MAIREntry {
	Noncache = 0x44, // Normal, inner/outer noncacheable
	Normal = 0xff,	 // Normal, inner/outer WB/WA/RA
	Device = 0x00,	 // Device memory, nGnRnE
};

constexpr uint64_t MAIR = (std::to_underlying(MAIREntry::Noncache) << std::to_underlying(MemType::Noncache) * 8) |
						  (std::to_underlying(MAIREntry::Normal) << std::to_underlying(MemType::Normal) * 8) |
						  (std::to_underlying(MAIREntry::Device) << std::to_underlying(MemType::Device) * 8);

enum Attributes : uint64_t {
	Valid = (1 << 0),
	Table = (1 << 1),
	// Index: 4:2
	NonSecure = (1 << 5),
	AccessPrivRW = (0b00 << 6),
	AccessRW = (0b01 << 6),
	AccessPrivRO = (0b10 << 6),
	AccessRO = (0b11 << 6),
	NonShareable = (0b00 << 8),
	InnerShareable = (0b11 << 8),
	OuterShareable = (0b10 << 8),
	AccessedFlag = (1 << 10),
	UnprivExecuteNever = (1ULL << 53),
	PrivExecuteNever = (1ULL << 54),
};

template<uint64_t EntrySize, uint64_t BaseVaddr>
struct PageTable {

	static constexpr auto entry_size = EntrySize;
	std::array<uint64_t, 512> data;

	uint64_t base() {
		return reinterpret_cast<uint64_t>(data.data());
	}

	void clear() {
		// Don't allow memset/memclr, since we don't have the MMU setup yet.
		// memset/memclr may use `dc zva` instructions, which will give an
		// alignment fault if the memory region is not setup as Normal yet.
		for (auto &d : data) {
			d = 0;
			asm("nop");
		}
	}

	void block_entry(uint64_t va, uint64_t pa, MemType type, uint64_t attribs) {
		uint64_t descriptor = pa;
		descriptor |= std::to_underlying(type) << MemTypeShift;
		descriptor |= Attributes::Valid;
		descriptor |= Attributes::AccessedFlag;

		if (type == MemType::Normal)
			descriptor |= InnerShareable;

		auto idx = (va - BaseVaddr) / EntrySize;
		data[idx] = descriptor;
	}

	void block_entry(uint64_t addr, MemType type, uint64_t attribs) {
		block_entry(addr, addr, type, attribs);
	}

	void fill_block_entries(uint64_t start_addr, uint64_t end_addr, MemType type, uint64_t attribs) {
		for (uint64_t a = start_addr; a < end_addr; a += entry_size)
			block_entry(a, a, type, attribs);
	}

	void table_entry(uint64_t va, auto &child) requires(child.entry_size == EntrySize / 512)
	{
		uint64_t descriptor = reinterpret_cast<uint64_t>(child.base());
		descriptor |= Attributes::Table;
		descriptor |= Attributes::Valid;
		descriptor |= Attributes::AccessedFlag;

		uint32_t idx = (va - BaseVaddr) / EntrySize;
		data[idx] = descriptor;
	}

	void table_entry(uint64_t addr, MemType type, uint64_t attribs) {
		table_entry(addr, addr, type, attribs);
	}
};

} // namespace MMU
