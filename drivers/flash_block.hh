#pragma once
#include "drivers/flash.hh"
#include <cstdint>
#include <span>

namespace mdrivlib
{

template<typename FlashIO>
concept CFlashIO = requires(uint32_t addr, std::span<const uint32_t> const_data, std::span<uint32_t> data) {
	{
		FlashIO::erase_sector(addr)
	} -> std::same_as<bool>;
	{
		FlashIO::write(const_data, addr)
	} -> std::same_as<bool>;
	{
		FlashIO::read(data, addr)
	} -> std::same_as<bool>;
};

// FlashBlock interfaces between WearLevel<> and the Flash IO functions.
//
// A sector, or block, is split into [cell_nr_] cells of equal size,
// each potentially containing an object of DataT with specified Alignment
// (example: AlignmentBits == 2 means align to 4 bytes)
template<typename DataT,
		 size_t SectorAddr,
		 size_t SectorSize,
		 CFlashIO FlashIO = mdrivlib::InternalFlash,
		 size_t AlignmentBits = 2>
struct FlashBlock {
	using data_t = DataT;
	static constexpr size_t data_size_ = sizeof(DataT);

	static constexpr bool pre_aligned = !(data_size_ & ((1u << AlignmentBits) - 1));
	static constexpr size_t aligned_data_size_ =
		pre_aligned ? data_size_ : ((data_size_ >> AlignmentBits) + 1) << AlignmentBits;
	static_assert(aligned_data_size_ < SectorSize, "Data plus alignment padding must fit into sector");

	static constexpr int cell_nr_ = SectorSize / aligned_data_size_;
	static_assert(cell_nr_ > 1, "Cell occupies more than half the sector, no wear-leveling possible.");

	bool read(data_t &data, int cell) {
		if (cell >= cell_nr_)
			return false;

		uint32_t addr = SectorAddr + cell * aligned_data_size_;

		std::span<uint32_t> stored_data{reinterpret_cast<uint32_t *>(&data), data_size_ / 4};
		return FlashIO::read(stored_data, addr);
	}

	bool write(const data_t &data, int cell) {
		if (cell >= cell_nr_)
			return false;

		uint32_t addr = SectorAddr + cell * aligned_data_size_;
		std::span<const uint32_t> stored_data{reinterpret_cast<const uint32_t *>(&data), data_size_ / 4};
		return FlashIO::write(stored_data, addr);
	}

	bool erase() {
		// Erase the entire block
		return FlashIO::erase_sector(SectorAddr);
	}

	// Verify all bits are 1's
	bool is_writeable(int cell) {
		if (cell >= cell_nr_)
			return false;

		data_t check_data;

		if (read(check_data, cell)) {
			auto p = reinterpret_cast<uint8_t *>(&check_data);
			for (unsigned i = 0; i < data_size_; i++) {
				if (p[i] != 0xFF)
					return false;
			}
			return true;
		}
		return false;
	}
};

} // namespace mdrivlib
