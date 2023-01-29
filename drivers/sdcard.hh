#pragma once
#include "drivers/rcc.hh"
#include "drivers/sdcard_conf.hh"
#include "stm32xx.h"
#include <span>

namespace mdrivlib
{

template<SDCardConfC ConfT>
struct SDCard {
	SD_HandleTypeDef hsd;
	enum class Status { NotInit, NoCard, Mounted } status = Status::NotInit;
	Pin card_det_pin;

	static constexpr uint32_t BlockSize = 512;

	SDCard() {
		RCC->SDMMC12CKSELR = 3; // HSI = 64MHz
		RCC_Enable::SDMMC1_::set();

		hsd.Instance = SDMMC1;
		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = SDMMC_BUS_WIDE_4B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
		hsd.Init.ClockDiv = 2; // 64MHz/2 / 2 = 16MHz, seems to be the max OSD32-BRK can handle reliably
		HAL_SD_DeInit(&hsd);

		Pin{ConfT::D0, PinMode::Alt};
		Pin{ConfT::D1, PinMode::Alt};
		Pin{ConfT::D2, PinMode::Alt};
		// Pin{ConfT::D3, PinMode::Alt};
		Pin{ConfT::CMD, PinMode::Alt};
		Pin{ConfT::SCLK, PinMode::Alt};

		init();
	}

	void init() {
		if (status == Status::NotInit) {
			set_status_nocard();
			if (card_det_pin.is_on()) {
				set_status_mounted();
				if (HAL_SD_Init(&hsd) != HAL_OK)
					set_status_nocard();
			}
		}
	}

	bool detect_card() {
		switch (status) {
			case Status::NoCard:
				if (card_det_pin.is_on())
					set_status_mounted();
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
	// Read from SD card into a generic data structure
	bool read(std::span<uint8_t> buf, uint32_t block_num) {
		constexpr uint32_t timeout = 0xFFFFFF;
		uint32_t bytes_to_read = buf.size_bytes();
		auto read_ptr = buf.data();

		if (bytes_to_read >= BlockSize) {
			uint32_t numblocks = bytes_to_read / BlockSize;
			auto err = HAL_SD_ReadBlocks(&hsd, read_ptr, block_num, numblocks, timeout);
			if (err != HAL_OK)
				return false;

			uint32_t bytes_read = numblocks * BlockSize;
			if (bytes_to_read == bytes_read)
				return true;

			//setup for reading the remainder
			bytes_to_read -= bytes_read;
			block_num += numblocks;
			read_ptr += bytes_read;
		}

		if (bytes_to_read > 0) {
			uint8_t _data[BlockSize];
			constexpr uint32_t numblocks = 1;
			auto err = HAL_SD_ReadBlocks(&hsd, _data, block_num, numblocks, timeout);
			if (err != HAL_OK)
				return false;

			uint8_t *src = _data;
			while (bytes_to_read--)
				*read_ptr++ = *src++;
		}
		return true;
	}

	bool write(const std::span<const uint8_t> buf, uint32_t block_num) {
		constexpr uint32_t timeout = 0xFFFFFF;
		uint32_t bytes_to_write = buf.size_bytes();

		// STM32 HAL is not const-correct, so we must perform a const cast
		auto buf_ptr = const_cast<uint8_t *>(buf.data());

		if (bytes_to_write >= BlockSize) {
			uint32_t numblocks = bytes_to_write / BlockSize;
			auto err = HAL_SD_WriteBlocks(&hsd, buf_ptr, block_num, numblocks, timeout);
			if (err != HAL_OK)
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
			uint8_t _data[512];
			uint8_t *dst = _data;
			for (unsigned i = 0; i < 512; i++) {
				if (i < bytes_to_write)
					*dst++ = *buf_ptr++;
				else
					*dst++ = 0;
			}

			constexpr uint32_t numblocks = 1;
			auto err = HAL_SD_WriteBlocks(&hsd, _data, block_num, numblocks, timeout);
			if (err != HAL_OK)
				return false;
		}
		return true;
	}

	private:
	void set_status_mounted() {
		status = Status::Mounted;
		card_det_pin.init(ConfT::D3, PinMode::Alt);
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
