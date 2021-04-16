#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "mdma_registers.hh"
#include "rcc.hh"
#include "stm32xx.h"

#include <cstddef>
struct MemoryTransferDefaultConfT {
	static constexpr unsigned channel = 0;
	static constexpr bool swap_bytes = true;
	static constexpr bool swap_halfwords = false;
	static constexpr bool swap_words = false;
};
namespace mdrivlib
{
// Todo: template on channel#
// And template on ConfigType:
// which ISRs are set
// which ISR gets the callback (or is it always CTC?)
// src/dst inc, dir, amount
//...
template<typename ConfT = MemoryTransferDefaultConfT>
struct MemoryTransfer {
	using MDMAX = target::MDMAx<ConfT::channel>;

	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	MemoryTransfer() {
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		InterruptManager::registerISR(MDMA_IRQn, 0, 2, [=]() {
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

	void config_transfer(void *dst, const void *src, size_t sz) {
		_transfer_size = sz;
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_src_addr = reinterpret_cast<uint32_t>(src);

		target::RCC_Enable::MDMA_::set();
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_size);
		MDMAX::BlockRepeatCount::write(0);

		MDMAX::SrcSize::write(MDMAX::Word);
		MDMAX::SrcIncDir::write(MDMAX::Up);
		MDMAX::SrcIncAmount::write(MDMAX::Word);
		MDMAX::DstSize::write(MDMAX::Word);
		MDMAX::DstIncDir::write(MDMAX::Up);
		MDMAX::DstIncAmount::write(MDMAX::Word);
		MDMAX::TransferLength::write(sz >= 128 ? 127 : sz - 1);
		if (sz > 127)
			MDMAX::TriggerMode::write(0b01); // Block transfer
		else
			MDMAX::TriggerMode::write(0b00); // Buffer transfer
		MDMAX::PaddingAlignmentMode::write(0b00);
		MDMAX::PackEnable::set();
		MDMAX::BufferableWriteMode::clear();

		if constexpr (ConfT::swap_bytes)
			MDMAX::ByteEndianessExchange::set();
		if constexpr (ConfT::swap_halfwords)
			MDMAX::HalfwordEndianessExchange::set();
		if constexpr (ConfT::swap_words)
			MDMAX::WordEndianessExchange::set();

		MDMAX::BufferTransferComplISRClear::set();
		MDMAX::BlockTransferComplISRClear::set();
		MDMAX::BlockRepeatTransferComplISRClear::set();
		MDMAX::ChannelTransferComplISRClear::set();
		MDMAX::TransferErrISRClear::set();

		_set_addrs();
		_set_addr_bus_bits();

		MDMAX::TransferErrISREnable::set();
		// MDMAX::BufferTransferComplISREnable::set();
		// MDMAX::BlockTransferComplISREnable::set();
		// MDMAX::BlockRepeatTransferComplISREnable::set();
		MDMAX::ChannelTransferComplISREnable::set();
	}

	void repeat_transfer_new_dst(void *dst) {
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_size);
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_set_addrs();
		MDMAX::Enable::set();
		MDMAX::SWRequest::set();
	}

	void start_transfer() {
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_size);
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
