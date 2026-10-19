#include "drivers/ospi_flash_driver.hh"
#include "drivers/pin.hh"
#include <algorithm>

namespace mdrivlib
{

namespace
{
// W25Qxx commands
enum : uint32_t {
	ResetEnable = 0x66,
	Reset = 0x99,
	ReadJedecId = 0x9F,
	ReadStatus1 = 0x05,
	ReadStatus2 = 0x35,
	WriteStatus2 = 0x31,
	WriteEnable = 0x06,
	FastRead = 0x0B,		// 1-1-1, 8 dummy cycles
	FastReadQuadIO = 0xEB,	// 1-4-4, mode byte + 4 dummy cycles
	PageProgram = 0x02,		// 1-1-1
	QuadPageProgram = 0x32, // 1-1-4
	SectorErase4K = 0x20,
	BlockErase32K = 0x52,
	BlockErase64K = 0xD8,
	ChipErase = 0xC7,
};

constexpr uint8_t Status1_Busy = 1 << 0;
constexpr uint8_t Status1_WriteEnabled = 1 << 1;
constexpr uint8_t Status2_QuadEnable = 1 << 1;

// Max times from the W25Q128JV datasheet, in ms
constexpr uint32_t PageProgramTimeout = 10;
constexpr uint32_t WriteStatusTimeout = 100;
constexpr uint32_t SectorEraseTimeout = 500;
constexpr uint32_t Block32KEraseTimeout = 2000;
constexpr uint32_t Block64KEraseTimeout = 3000;
constexpr uint32_t ChipEraseTimeout = 250'000;
} // namespace

OSpiFlash::OSpiFlash(const OSPIFlashConfig &config)
	: defs{config} {

	const bool is_ospi2 = defs.ospi_num == 2;
	handle.Instance = is_ospi2 ? OCTOSPI2 : OCTOSPI1;

	auto kernel_clock = defs.kernel_clock;
	kernel_clock.init(is_ospi2 ? 49 : 48);

	// The IO manager's reset state is direct mode: OCTOSPIn on IO port n
	__HAL_RCC_OSPIIOM_CLK_ENABLE();

	if (is_ospi2) {
		__HAL_RCC_OSPI2_CLK_ENABLE();
		__HAL_RCC_OSPI2_FORCE_RESET();
		__HAL_RCC_OSPI2_RELEASE_RESET();
	} else {
		__HAL_RCC_OSPI1_CLK_ENABLE();
		__HAL_RCC_OSPI1_FORCE_RESET();
		__HAL_RCC_OSPI1_RELEASE_RESET();
	}

	Pin{defs.cs, PinMode::Alt, PinPull::Up, PinPolarity::Normal, PinSpeed::VeryHigh};
	Pin{defs.clk, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
	Pin{defs.io0, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
	Pin{defs.io1, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};

	// Hold /WP and /HOLD high until the chip is in quad mode (where they're IO2 and IO3)
	if (defs.io2.gpio != GPIO::Unused)
		Pin{defs.io2, PinMode::Output}.high();
	if (defs.io3.gpio != GPIO::Unused)
		Pin{defs.io3, PinMode::Output}.high();

	handle.Init.FifoThreshold = 4;
	handle.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
	handle.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
	handle.Init.DeviceSize = 32 - __builtin_clz(defs.flash_size_bytes - 1);
	handle.Init.ChipSelectHighTime = 2;
	handle.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
	handle.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
	handle.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
	handle.Init.ClockPrescaler = defs.clock_division;
	handle.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
	handle.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
	handle.Init.ChipSelectBoundary = 0;
	handle.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
	handle.Init.MaxTran = 0;
	handle.Init.Refresh = 0;

	if (HAL_OSPI_Init(&handle) != HAL_OK)
		return;

	reset();

	if (defs.io_mode == OSPIFlashConfig::QuadSPI && enable_quad_mode()) {
		quad_mode = true;
		Pin{defs.io2, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		Pin{defs.io3, PinMode::Alt, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
	}
}

OSPI_RegularCmdTypeDef OSpiFlash::command(uint32_t instruction) {
	return OSPI_RegularCmdTypeDef{
		.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
		.FlashId = HAL_OSPI_FLASH_ID_1,
		.Instruction = instruction,
		.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
		.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
		.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
		.Address = 0,
		.AddressMode = HAL_OSPI_ADDRESS_NONE,
		.AddressSize = HAL_OSPI_ADDRESS_24_BITS,
		.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
		.AlternateBytes = 0,
		.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
		.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS,
		.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
		.DataMode = HAL_OSPI_DATA_NONE,
		.NbData = 0,
		.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
		.DummyCycles = 0,
		.DQSMode = HAL_OSPI_DQS_DISABLE,
		.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
	};
}

bool OSpiFlash::send(OSPI_RegularCmdTypeDef &cmd, uint32_t timeout) {
	return HAL_OSPI_Command(&handle, &cmd, timeout) == HAL_OK;
}

bool OSpiFlash::read_register(uint32_t instruction, uint8_t *value) {
	auto cmd = command(instruction);
	cmd.DataMode = HAL_OSPI_DATA_1_LINE;
	cmd.NbData = 1;
	return send(cmd) && HAL_OSPI_Receive(&handle, value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK;
}

bool OSpiFlash::reset() {
	auto cmd = command(ResetEnable);
	if (!send(cmd))
		return false;

	cmd.Instruction = Reset;
	if (!send(cmd))
		return false;

	// tRST = 30us
	HAL_Delay(1);
	return true;
}

bool OSpiFlash::write_enable() {
	auto cmd = command(WriteEnable);
	if (!send(cmd))
		return false;

	uint8_t status = 0;
	return read_register(ReadStatus1, &status) && (status & Status1_WriteEnabled);
}

bool OSpiFlash::wait_until_ready(uint32_t timeout) {
	auto start = HAL_GetTick();
	uint8_t status = Status1_Busy;
	do {
		if (!read_register(ReadStatus1, &status))
			return false;
		if (!(status & Status1_Busy))
			return true;
	} while ((HAL_GetTick() - start) <= timeout);

	return false;
}

bool OSpiFlash::enable_quad_mode() {
	uint8_t status2 = 0;
	if (!read_register(ReadStatus2, &status2))
		return false;

	if (status2 & Status2_QuadEnable)
		return true;

	// QE is non-volatile, so this only happens the first time on a new chip
	if (!write_enable())
		return false;

	auto cmd = command(WriteStatus2);
	cmd.DataMode = HAL_OSPI_DATA_1_LINE;
	cmd.NbData = 1;
	status2 |= Status2_QuadEnable;
	if (!send(cmd) || HAL_OSPI_Transmit(&handle, &status2, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	if (!wait_until_ready(WriteStatusTimeout))
		return false;

	return read_register(ReadStatus2, &status2) && (status2 & Status2_QuadEnable);
}

bool OSpiFlash::read_chip_id(uint32_t *chip_id) {
	auto cmd = command(ReadJedecId);
	cmd.DataMode = HAL_OSPI_DATA_1_LINE;
	cmd.NbData = 3;

	uint8_t id[3]{};
	if (!send(cmd) || HAL_OSPI_Receive(&handle, id, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	*chip_id = id[0] | (id[1] << 8) | (id[2] << 16);
	return true;
}

bool OSpiFlash::check_chip_id(uint32_t expected_id, uint32_t mask) {
	for (unsigned tries = 0; tries < 100; tries++) {
		uint32_t id = 0;
		if (read_chip_id(&id) && (id & mask) == expected_id)
			return true;
		HAL_Delay(1);
	}
	return false;
}

bool OSpiFlash::read(uint8_t *data, uint32_t read_addr, uint32_t num_bytes, UseInterruptFlags) {
	if (num_bytes == 0)
		return true;
	if (read_addr >= defs.flash_size_bytes || num_bytes > defs.flash_size_bytes - read_addr)
		return false;

	OSPI_RegularCmdTypeDef cmd;
	if (quad_mode) {
		cmd = command(FastReadQuadIO);
		cmd.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
		// Mode bits M7-M0 = 0: not continuous read mode
		cmd.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
		cmd.AlternateBytes = 0x00;
		cmd.DummyCycles = 4;
		cmd.DataMode = HAL_OSPI_DATA_4_LINES;
	} else {
		cmd = command(FastRead);
		cmd.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
		cmd.DummyCycles = 8;
		cmd.DataMode = HAL_OSPI_DATA_1_LINE;
	}
	cmd.Address = read_addr;
	cmd.NbData = num_bytes;

	return send(cmd) && HAL_OSPI_Receive(&handle, data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK;
}

bool OSpiFlash::write_page(const uint8_t *data, uint32_t write_addr, uint32_t num_bytes, UseInterruptFlags) {
	if (num_bytes == 0)
		return true;

	// Must not cross a page boundary
	if (num_bytes > PageSize || (write_addr / PageSize) != ((write_addr + num_bytes - 1) / PageSize))
		return false;

	if (!write_enable())
		return false;

	auto cmd = command(quad_mode ? QuadPageProgram : PageProgram);
	cmd.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
	cmd.Address = write_addr;
	cmd.DataMode = quad_mode ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
	cmd.NbData = num_bytes;

	if (!send(cmd))
		return false;
	if (HAL_OSPI_Transmit(&handle, const_cast<uint8_t *>(data), HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	return wait_until_ready(PageProgramTimeout);
}

bool OSpiFlash::write(const uint8_t *data, uint32_t write_addr, uint32_t num_bytes) {
	if (write_addr >= defs.flash_size_bytes || num_bytes > defs.flash_size_bytes - write_addr)
		return false;

	while (num_bytes) {
		// Up to the end of the page
		uint32_t chunk = std::min<uint32_t>(num_bytes, PageSize - (write_addr % PageSize));
		if (!write_page(data, write_addr, chunk))
			return false;
		data += chunk;
		write_addr += chunk;
		num_bytes -= chunk;
	}
	return true;
}

bool OSpiFlash::erase(uint32_t size, uint32_t base_addr, UseInterruptFlags) {
	uint32_t instruction;
	uint32_t timeout;

	switch (size) {
		case SECTOR:
			instruction = SectorErase4K;
			timeout = SectorEraseTimeout;
			break;
		case BLOCK_32K:
			instruction = BlockErase32K;
			timeout = Block32KEraseTimeout;
			break;
		case BLOCK_64K:
			instruction = BlockErase64K;
			timeout = Block64KEraseTimeout;
			break;
		case ENTIRE_CHIP:
			instruction = ChipErase;
			timeout = ChipEraseTimeout;
			break;
		default:
			return false;
	}

	if (size != ENTIRE_CHIP && base_addr >= defs.flash_size_bytes)
		return false;

	if (!write_enable())
		return false;

	auto cmd = command(instruction);
	if (size != ENTIRE_CHIP) {
		cmd.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
		cmd.Address = base_addr & ~(size - 1);
	}

	if (!send(cmd))
		return false;

	return wait_until_ready(timeout);
}

} // namespace mdrivlib
