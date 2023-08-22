#pragma once
#include "drivers/cache.hh"
#include "drivers/interrupt.hh"
#include "drivers/mdma_registers.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"
#include <cstddef>

namespace mdrivlib
{

struct MemoryTransferDefaultConfT {
	static constexpr unsigned channel = 0;

	// Endianess
	static constexpr bool swap_bytes = false;
	static constexpr bool swap_halfwords = false;
	static constexpr bool swap_words = false;

	// Data sizes
	enum Sizes { Byte = 0b00, HalfWord = 0b01, Word = 0b10, DoubleWord = 0b11 };
	static constexpr uint32_t src_data_size = Word;
	static constexpr uint32_t dst_data_size = Word;
	static constexpr uint32_t src_data_inc = Word;
	static constexpr uint32_t dst_data_inc = Word;
	static constexpr uint32_t src_burst = 0b000;
	static constexpr uint32_t dst_burst = 0b000;

	static constexpr uint32_t src_inc_each_block = 0;
	static constexpr uint32_t dst_inc_each_block = 0;

	enum Directions { DoNotInc = 0b00, Up = 0b10, Down = 0b11 };
	static constexpr uint32_t src_data_dir = Up;
	static constexpr uint32_t dst_data_dir = Up;

	enum Priority { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };
	static constexpr uint32_t PriorityLevel = Medium;

	static constexpr bool bufferable_write_mode = false;
};

// Todo: create `register_channel_isr` and `enable_global_isr` like HSEM does, to handle multiple MemoryTransfer active
// at once, on different channels.
// - Add ability to choose which ISR flags are set (CTC, BRTC, BTC, etc), and which ISR gets the callback (or is it
// always CTC?)
template<typename ConfT = MemoryTransferDefaultConfT>
struct MemoryTransfer {
	using MDMAX = MDMA_<ConfT::channel>;

	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_block_size;
	uint32_t _num_blocks;

	uint32_t _fillval;

	MemoryTransfer() {
		RCC_Enable::MDMA_::set();
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		InterruptManager::register_and_start_isr(MDMA_IRQn, 0, 2, [=]() {
			// if (MDMAX::BlockRepeatTransferComplISRFlag::read() && MDMAX::BlockRepeatTransferComplISREnable::read()) {
			// 	MDMAX::BlockRepeatTransferComplISRClear::set();
			// }

			// if (MDMAX::BlockTransferComplISRFlag::read() && MDMAX::BlockTransferComplISREnable::read()) {
			// 	MDMAX::BlockTransferComplISRClear::set();
			// }

			if (MDMAX::ChannelTransferComplISRFlag::read() && MDMAX::ChannelTransferComplISREnable::read()) {
				MDMAX::ChannelTransferComplISRClear::set();
				callback();
			}

			// if (MDMAX::BufferTransferComplISRFlag::read() && MDMAX::BufferTransferComplISREnable::read()) {
			// 	MDMAX::BufferTransferComplISRClear::set();
			// }

			if (MDMAX::TransferErrISRFlag::read() && MDMAX::TransferErrISREnable::read()) {
				MDMAX::TransferErrISRClear::set();
			}
		});
	}

	struct Area {
		int32_t x0, x1, y0, y1;
	};

	void config_fill_2d(void *dst, uint32_t fillval, uint32_t dst_width, Area area) {
		constexpr uint32_t data_size = ConfT::dst_data_size == ConfT::Byte	   ? 1
									 : ConfT::dst_data_size == ConfT::HalfWord ? 2
									 : ConfT::dst_data_size == ConfT::Word	   ? 4
																			   : 8;

		_dst_addr = reinterpret_cast<uint32_t>(dst) + (area.y0 * dst_width * data_size);

		_fillval = fillval;

		// Todo: needed? Maybe always fill to DoubleWord
		if (ConfT::src_data_size == ConfT::HalfWord)
			_fillval |= (_fillval << 16);

		_src_addr = reinterpret_cast<uint32_t>(&_fillval); // invariant
		SystemCache::clean_dcache_by_addr(_src_addr);

		_num_blocks = area.y1 - area.y0 - 1;
		_transfer_block_size = (area.x1 - area.x0) * data_size;

		MDMAX::SrcAddrUpdateValue::write(0); // invariant
		MDMAX::DstAddrUpdateValue::write(dst_width * data_size - _transfer_block_size);
		MDMAX::BlockRepeatDstUpdateMode::write(MDMAX::AddDestUpdateVal);

		/// this should be same as _config()
		MDMAX::Enable::clear();

		MDMAX::SrcSize::write(ConfT::src_data_size);	 // invariant
		MDMAX::SrcIncDir::write(MDMAX::DoNotInc);		 // invariant
		MDMAX::SrcIncAmount::write(ConfT::src_data_inc); // invariant

		MDMAX::DstSize::write(ConfT::dst_data_size);	 // invariant
		MDMAX::DstIncDir::write(MDMAX::Up);				 // invariant
		MDMAX::DstIncAmount::write(ConfT::dst_data_inc); // invariant

		MDMAX::SrcBurst::write(ConfT::src_burst); // invariant
		MDMAX::DstBurst::write(ConfT::dst_burst); // invariant

		// These are done in start_transfer:
		// MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		// MDMAX::BlockRepeatCount::write(_num_blocks);

		MDMAX::TriggerMode::write(MDMAX::RepeatedBlock); // invariant

		MDMAX::TransferLength::write(_transfer_block_size >= 128 ? 127 : _transfer_block_size - 1);

		MDMAX::PaddingAlignmentMode::write(0b00); // invariant
		MDMAX::PackEnable::set();				  // invariant
		MDMAX::SWRequestMode::set();			  // invariant

		if constexpr (ConfT::bufferable_write_mode)
			MDMAX::BufferableWriteMode::set();
		else
			MDMAX::BufferableWriteMode::clear(); // invariant

		// Clear all ISRs
		MDMAX::BufferTransferComplISRClear::set();
		MDMAX::BlockTransferComplISRClear::set();
		MDMAX::BlockRepeatTransferComplISRClear::set();
		MDMAX::ChannelTransferComplISRClear::set();
		MDMAX::TransferErrISRClear::set();

		_set_addrs();
		_set_addr_bus_bits();

		// Set certain ISRs (todo: allow config)
		MDMAX::TransferErrISREnable::set();
		// MDMAX::BufferTransferComplISREnable::set();
		// MDMAX::BlockTransferComplISREnable::set();
		// MDMAX::BlockRepeatTransferComplISREnable::set();
		MDMAX::ChannelTransferComplISREnable::set();
	}

	// Configures a memory transfer using ConfT and given parameters.
	// Typically, call this just once, as it inits many things.
	// The data will be split up into multiple blocks if necessary
	void config_transfer(void *dst, const void *src, size_t sz) {
		// Calc size of each block, and number of blocks
		_transfer_block_size = sz;
		_num_blocks = 1;
		while (_transfer_block_size >= 0x8000U) {
			_num_blocks++;
			_transfer_block_size = sz / _num_blocks;
		}
		// Deal with leftover bytes due to _transfer_block_size * _num_blocks < sz
		// Todo: test this!
		while (_transfer_block_size * _num_blocks < sz)
			_transfer_block_size += (ConfT::src_data_size == 0b00) ? 1
								  : (ConfT::src_data_size == 0b01) ? 2
								  : (ConfT::src_data_size == 0b10) ? 4
																   : 8;

		// num_blocks is zero-based
		_num_blocks--;
		_config(dst, src, sz);
	}

	void _config(void *dst, const void *src, size_t sz) {
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_src_addr = reinterpret_cast<uint32_t>(src);

		MDMAX::Enable::clear();

		MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		MDMAX::BlockRepeatCount::write(_num_blocks);

		MDMAX::SrcSize::write(ConfT::src_data_size);
		MDMAX::SrcIncDir::write(MDMAX::Up);
		MDMAX::SrcIncAmount::write(ConfT::src_data_inc);
		MDMAX::DstSize::write(ConfT::dst_data_size);
		MDMAX::DstIncDir::write(MDMAX::Up);
		MDMAX::DstIncAmount::write(ConfT::dst_data_inc);
		MDMAX::SrcBurst::write(ConfT::src_burst);
		MDMAX::DstBurst::write(ConfT::dst_burst);

		MDMAX::TransferLength::write(sz >= 128 ? 127 : sz - 1);

		// Todo: try if we can just set TriggerMode to 0b11 (transfer all)
		if (_num_blocks > 0)
			MDMAX::TriggerMode::write(0b10); // Repeated Block transfer
		else if (sz > 127)
			MDMAX::TriggerMode::write(0b01); // Block transfer
		else
			MDMAX::TriggerMode::write(0b00); // Buffer transfer

		MDMAX::PaddingAlignmentMode::write(0b00);
		MDMAX::PackEnable::set();

		// Use software request mode for memory-to-memory transfers
		MDMAX::SWRequestMode::set();

		if constexpr (ConfT::bufferable_write_mode)
			MDMAX::BufferableWriteMode::set();
		else
			MDMAX::BufferableWriteMode::clear();

		if constexpr (ConfT::swap_bytes)
			MDMAX::ByteEndianessExchange::set();
		if constexpr (ConfT::swap_halfwords)
			MDMAX::HalfwordEndianessExchange::set();
		if constexpr (ConfT::swap_words)
			MDMAX::WordEndianessExchange::set();

		MDMAX::PriorityLevel::write(ConfT::PriorityLevel);

		// Clear all ISRs
		MDMAX::BufferTransferComplISRClear::set();
		MDMAX::BlockTransferComplISRClear::set();
		MDMAX::BlockRepeatTransferComplISRClear::set();
		MDMAX::ChannelTransferComplISRClear::set();
		MDMAX::TransferErrISRClear::set();

		_set_addrs();
		_set_addr_bus_bits();

		// Set certain ISRs (todo: allow config)
		MDMAX::TransferErrISREnable::set();
		// MDMAX::BufferTransferComplISREnable::set();
		// MDMAX::BlockTransferComplISREnable::set();
		// MDMAX::BlockRepeatTransferComplISREnable::set();
		MDMAX::ChannelTransferComplISREnable::set();
	}

	void repeat_transfer_new_dst(void *dst) {
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		MDMAX::BlockRepeatCount::write(_num_blocks);
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_set_addrs();
		MDMAX::Enable::set();
		MDMAX::SWRequest::set();
	}

	void start_transfer() {
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		MDMAX::BlockRepeatCount::write(_num_blocks);
		_set_addrs();
		MDMAX::Enable::set();
		// Debug point:
		// if (MDMAX::RequestIsActive::read())
		// 	while (1)
		// 		;
		MDMAX::SWRequest::set();
	}

private:
	void _set_addrs() {
		MDMAX::SrcAddr::write(_src_addr);
		MDMAX::DstAddr::write(_dst_addr);
	}

	void _set_addr_bus_bits() {
		const uint32_t src_bus = _src_addr & 0xFF000000;
		MDMAX::SrcBusIsAHBTCM::write(src_bus == 0x20000000 || src_bus == 0);

		const uint32_t dst_bus = _dst_addr & 0xFF000000;
		MDMAX::DstBusIsAHBTCM::write(dst_bus == 0x20000000 || dst_bus == 0);
	}
};
} // namespace mdrivlib
