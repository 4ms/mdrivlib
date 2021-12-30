#pragma once
#include "qspi_flash_registers.h"
#include "qspi_flash_struct.hh"
#include "stm32xx.h"

namespace mdrivlib
{

class QSpiFlash {

public:
	enum ErasableSizes : uint32_t {
		SECTOR = QSPI_SECTOR_SIZE,
		BLOCK_32K = QSPI_32KBLOCK_SIZE,
		BLOCK_64K = QSPI_64KBLOCK_SIZE,
		ENTIRE_CHIP = 0xFFFFFFFF
	};

	enum FlashStatus { STATUS_READY, STATUS_WIP, STATUS_RXING, STATUS_RX_COMPLETE, STATUS_TXING, STATUS_TX_COMPLETE };

	enum UseInterruptFlags { EXECUTE_FOREGROUND, EXECUTE_BACKGROUND };

private:
	QSPI_HandleTypeDef handle;
	QSPI_CommandTypeDef s_command;

	const QSPIFlashConfig &defs;
	const uint32_t data_mode;
	const int quad_write_cmd;
	const int quad_read_dummy_cycles;

	HAL_StatusTypeDef WriteEnable();
	void GPIO_Init_IO0_IO1();
	void GPIO_Init_IO2_IO3_AF();
	HAL_StatusTypeDef AutoPollingMemReady(uint32_t Timeout);
	HAL_StatusTypeDef AutoPollingMemReady_IT();
	HAL_StatusTypeDef EnterMemory_QPI();
	void init_command(QSPI_CommandTypeDef *s_command);

	bool done_TXing() {
		return QSPI_status == STATUS_TX_COMPLETE;
	}

	uint8_t test_encode_num(uint32_t num);

public:
	QSpiFlash(const QSPIFlashConfig &config_defs);

	bool is_ready() {
		return QSPI_status == STATUS_READY;
	}

	bool Test();
	bool Test_Sector(uint8_t sector_num);

	uint32_t get_64kblock_addr(int block64k_num);
	uint32_t get_32kblock_addr(int block32k_num);
	uint32_t get_sector_addr(int sector_num);

	HAL_StatusTypeDef Reset();

	bool Read(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt);
	bool Read_Background(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes) {
		return Read(pData, read_addr, num_bytes, EXECUTE_BACKGROUND);
	}

	bool Write(uint8_t *pData, uint32_t write_addr, uint32_t num_bytes);
	bool Write_Page(uint8_t *pData, uint32_t write_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt);

	bool Erase(uint32_t size, uint32_t base_addr, UseInterruptFlags use_interrupt);
	bool Erase_Background(ErasableSizes size, uint32_t base_addr) {
		return Erase(size, base_addr, EXECUTE_FOREGROUND);
	}
	bool Erase_Block_Background(uint32_t base_addr) {
		return Erase(BLOCK_32K, base_addr, EXECUTE_BACKGROUND);
	}

	bool read_config(uint32_t *data);
	bool read_chip_id(uint32_t *chip_id_ptr);

	// public for use in callbacks and IRQ
	volatile enum FlashStatus QSPI_status = STATUS_READY;
	static QSpiFlash *instance_;
};

} // namespace mdrivlib
