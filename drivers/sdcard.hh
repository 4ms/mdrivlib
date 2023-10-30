#pragma once
#include "drivers/rcc.hh"
#include "drivers/sdcard_conf.hh"
#include "drivers/sdcard_target.hh"
#include "drivers/stm32xx.h"
#include <span>

namespace mdrivlib
{

template<SDCardConfC ConfT>
struct SDCard {
	SD_HandleTypeDef hsd;
	enum class Status { NotInit, NoCard, Mounted } status = Status::NotInit;
	Pin card_det_pin;

	using Target = SDMMCTarget<ConfT>;

	static constexpr uint32_t BlockSize = 512;

	SDCard() {
		Target::setup_hal_handle(hsd);

		//FIXME: handle other SDMMC#
		RCC_Enable::SDMMC1_::set();

		hsd.Instance = SDMMC1;
		HAL_SD_DeInit(&hsd);

		Pin{ConfT::D0, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		Pin{ConfT::D1, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		Pin{ConfT::D2, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		Pin{ConfT::CMD, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		Pin{ConfT::SCLK, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};

		//Note: We do not init D3 as AltFunc; it's used as card detect

		init();
	}

	void init() {
		if (status != Status::NotInit)
			return;

		set_status_nocard();
		if (card_det_pin.is_on()) {
			set_status_mounted();
			if (HAL_SD_Init(&hsd) != HAL_OK) {
				set_status_nocard();
				return;
			}
			if constexpr (ConfT::width == DefaultSDCardConf::Wide4)
				Target::config_widebus(&hsd);
		}
	}

	uint32_t get_card_num_blocks() {
		if (!detect_card())
			return 0;

		HAL_SD_CardInfoTypeDef info;
		auto err = HAL_SD_GetCardInfo(&hsd, &info);
		if (err != HAL_OK)
			return 0;

		return info.BlockNbr;
	}

	bool detect_card() {
		switch (status) {
			case Status::NoCard:
				if (card_det_pin.is_on()) {
					set_status_mounted();
					HAL_SD_DeInit(&hsd);
					if (HAL_SD_Init(&hsd) != HAL_OK) {
						// printf_("Cannot re-mount, err %d\n", (unsigned)HAL_SD_GetError(&hsd));
						set_status_nocard();
					}
				}
				break;

			case Status::Mounted: {
				hsd.ErrorCode = HAL_SD_ERROR_NONE;
				auto cardstate = HAL_SD_GetCardState(&hsd);
				if (HAL_SD_GetError(&hsd) != HAL_SD_ERROR_NONE || cardstate == HAL_SD_CARD_DISCONNECTED ||
					cardstate == HAL_SD_CARD_ERROR)
					set_status_nocard();
			} break;

			case Status::NotInit:
				init();
				break;
		}
		return (status == Status::Mounted);
	}

	bool try_mount() {
		if (status == Status::Mounted)
			return true;
		if (status == Status::NoCard) {
			if (card_det_pin.is_on()) {
				set_status_mounted();
				return true;
			}
		}
		return false;
	}

	void check_status() {
		if (status != Status::Mounted)
			return;

		hsd.ErrorCode = HAL_SD_ERROR_NONE;
		auto cardstate = HAL_SD_GetCardState(&hsd);
		if (HAL_SD_GetError(&hsd) != HAL_SD_ERROR_NONE || cardstate == HAL_SD_CARD_DISCONNECTED ||
			cardstate == HAL_SD_CARD_ERROR)
			set_status_nocard();
	}

	bool wait_until_ready() {
		if (status != Status::Mounted)
			return false;

		constexpr uint32_t timeout = 1000;
		uint32_t tm = HAL_GetTick();
		while ((HAL_GetTick() - tm) < timeout) {
			if (HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_TRANSFER)
				return true; //Transfer OK
			if (HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_READY)
				return true; //Card is ready<<<Check this, it's not in ST BSP
		}

		return false;
	}

	// Read from SD card into a generic data structure
	bool read(std::span<uint8_t> buf, uint32_t block_num) {
		// constexpr uint32_t timeout = 2000; //2 seconds
		uint32_t bytes_to_read = buf.size_bytes();
		auto read_ptr = buf.data();

		bool buf_is_aligned = !((uint32_t)buf.data() & 0b11);

		if (buf_is_aligned && (bytes_to_read >= BlockSize)) {
			uint32_t numblocks = bytes_to_read / BlockSize;

			if (!wait_until_ready())
				return false;
			if (!Target::read(&hsd, read_ptr, block_num, numblocks))
				return false;
			if (!wait_until_ready())
				return false;

			uint32_t bytes_read = numblocks * BlockSize;
			if (bytes_to_read == bytes_read)
				return true;

			//setup for reading the remainder
			bytes_to_read -= bytes_read;
			block_num += numblocks;
			read_ptr += bytes_read;
		}

		while (bytes_to_read > 0) {
			alignas(4) uint8_t aligned_data[BlockSize];
			constexpr uint32_t numblocks = 1;

			if (!wait_until_ready())
				return false;
			if (!Target::read(&hsd, aligned_data, block_num, numblocks))
				return false;
			if (!wait_until_ready())
				return false;

			uint32_t bytes_to_copy = std::min(bytes_to_read, BlockSize);
			for (unsigned i = 0; i < bytes_to_copy; i++) {
				*read_ptr++ = aligned_data[i];
			}
			bytes_to_read -= bytes_to_copy;
			block_num++;
		}
		return true;
	}

	bool write(const std::span<const uint8_t> buf, uint32_t block_num) {
		// constexpr uint32_t timeout = 0xFFFFFF;
		uint32_t bytes_to_write = buf.size_bytes();

		// STM32 HAL is not const-correct, so we must perform a const cast
		auto buf_ptr = const_cast<uint8_t *>(buf.data());

		if (bytes_to_write >= BlockSize) {
			uint32_t numblocks = bytes_to_write / BlockSize;
			// if (HAL_SD_WriteBlocks(&hsd, buf_ptr, block_num, numblocks, timeout) != HAL_OK)
			// 	return false;
			if (!wait_until_ready())
				return false;
			if (!Target::write(&hsd, buf_ptr, block_num, numblocks))
				return false;
			if (!wait_until_ready())
				return false;

			uint32_t bytes_written = numblocks * 512;
			if (bytes_to_write == bytes_written)
				return true;

			//setup for writing the remainder (less than a whole block)
			bytes_to_write -= bytes_written;
			block_num += numblocks;
			buf_ptr += bytes_written;
		}

		// Handle writing less than a whole block, by copying and padding
		if (bytes_to_write > 0 && bytes_to_write < 512) {
			alignas(4) uint8_t _data[512];
			uint8_t *dst = _data;
			for (unsigned i = 0; i < 512; i++) {
				if (i < bytes_to_write)
					*dst++ = *buf_ptr++;
				else
					*dst++ = 0;
			}

			constexpr uint32_t numblocks = 1;
			// if (HAL_SD_WriteBlocks(&hsd, _data, block_num, numblocks, timeout) != HAL_OK)
			// 	return false;
			if (!wait_until_ready())
				return false;
			if (!Target::write(&hsd, buf_ptr, block_num, numblocks))
				return false;
			if (!wait_until_ready())
				return false;
		}
		return true;
	}

	uint32_t test_rx_speed(uint32_t block_num = 0, uint32_t end_block = 10 * 2048) {
		alignas(4) uint8_t _data[512];

		uint32_t start_tm = HAL_GetTick();
		while (block_num < end_block) {
			if (!Target::read(&hsd, _data, block_num++, 1))
				return 0;
		}
		uint32_t stop_tm = HAL_GetTick();
		return stop_tm - start_tm;
	}

private:
	void set_status_mounted() {
		status = Status::Mounted;
		card_det_pin.init(ConfT::D3, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh);
	}

	void set_status_nocard() {
		status = Status::NoCard;
		//TODO: Check if we need to toggle pin state, we might be able to read the pin
		// even when its in AF mode
		card_det_pin.init(ConfT::D3, PinMode::Input, PinPull::None);
		HAL_Delay(1);
	}
};

} // namespace mdrivlib
