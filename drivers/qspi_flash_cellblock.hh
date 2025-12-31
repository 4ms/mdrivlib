#pragma once
#include "qspi_flash_driver.hh"
#include "util/math.hh"

namespace mdrivlib
{

// FlashBlock<qspi.get_32kblock_addr(3), MyDataStruct, QSPI_32KBLOCK_SIZE, QSPI_PAGE_SIZE> my_block;

// Wrapper reader/writer for a QSPI Flash block of size BlockSize. The block is split into
// [NumCells] cells of equal size, each potentially containing an object of
// Data, and aligned to page boundaries
template<int BlockAddr, class DataT, mdrivlib::QSpiFlash::ErasableSizes BlockSize, int PageSize = QSPI_PAGE_SIZE>
struct FlashCellBlock {
	QSpiFlash &qspi_;

	FlashCellBlock(QSpiFlash &qspi)
		: qspi_(qspi) {
		if (!qspi_.check_chip_id(0x180001, 0x00180001)) //182001 or 186001 or 1840EF
			__BKPT(1);
	}

	using data_t = DataT;
	static constexpr int DataSize = sizeof(DataT);
	static constexpr int PageSizeBits = MathTools::Log2<PageSize>::val;
	static constexpr int AlignedDataSize = ((DataSize >> PageSizeBits) + 1) << PageSizeBits;
	static constexpr int NumCells = BlockSize / AlignedDataSize;

	static_assert(MathTools::is_power_of_2(PageSize));
	static_assert(AlignedDataSize < BlockSize);
	static_assert(BlockSize < 64 * 1024 * 1024, "BlockSize of ENTIRE_CHIP is not supported");

	bool read(DataT *data, int cell = 0) {
		if (cell >= NumCells)
			return false;
		uint32_t addr = BlockAddr + cell * AlignedDataSize;
		while (!qspi_.is_ready())
			;
		bool read_ok = qspi_.read(reinterpret_cast<uint8_t *>(data), addr, DataSize);
		while (!qspi_.is_ready())
			;
		return read_ok;
	}

	bool write(DataT const *data, int cell = 0) {
		if (cell >= NumCells)
			return false;
		uint32_t addr = BlockAddr + cell * AlignedDataSize;
		while (!qspi_.is_ready())
			;
		return qspi_.write(reinterpret_cast<uint8_t const *>(data), addr, DataSize);
	}

	bool erase() {
		while (!qspi_.is_ready())
			;
		return qspi_.erase(BlockSize, BlockAddr);
	}

	// Verify all bits are 1's
	bool is_writeable(int cell) {
		if (cell >= NumCells)
			return false;
		static uint8_t check[DataSize];
		if (read(reinterpret_cast<DataT *>(check), cell)) {
			for (int i = 0; i < DataSize; i++) {
				if (check[i] != 0xFF)
					return false;
			}
			return true;
		}
		return false;
	}
};

} // namespace mdrivlib
