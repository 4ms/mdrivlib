#include "adc_builtin_conf.hh"
#include "stm32xx.h"
namespace mdrivlib
{

//dma_config.hh

struct DMAConfig {
	uint32_t DMAx;
	int32_t stream = -1;
	int32_t channel = -1;
	int32_t requestnum = -1;
	IRQn_Type IRQn;
	uint32_t pri = 3;
	uint32_t subpri;

	enum Direction { Mem2Mem, Mem2Periph, Periph2Mem };
	Direction dir = Mem2Periph;

	bool circular = false;

	enum TransferSize { Byte, HalfWord, Word };
	TransferSize transfer_size_mem = Byte;	  // Dest, in M2M
	TransferSize transfer_size_periph = Byte; // Source, in M2M

	enum Priority { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };
	uint8_t dma_priority = Low;

	bool mem_inc = true;
	bool periph_inc = false;
	bool half_transfer_interrupt_enable = false;

	bool enable_fifo = true;
	enum FifoThreshold { Fifo1QuarterFull, FifoHalfFull, Fifo3QuarterFull, FifoFull };
	FifoThreshold fifo_threshold = FifoHalfFull;

	template<auto member>
	constexpr DMAConfig &set(auto val) {
		this->*member = val;
		return *this;
	};
};

//adc_builtin_config.hh

struct AdcPeriphConfig {
	AdcPeriphNum adc_periph_num = AdcPeriphNum::_1;

	// Data type
	using ValueT = uint16_t;
	// enum DataSize { Byte, HalfWord, Word };
	// static constexpr DataSize data_size = HalfWord;

	AdcResolution resolution = Bits12;

	AdcDataAlign align = AlignLeft;

	// Ovesampling
	bool oversample = false;
	uint32_t oversampling_ratio = 0;
	AdcOversampleRightBitShift oversampling_right_bitshift = NoShift;
	// TODO: other os config opts?

	// Clock
	AdcClockSourceDiv clock_div = PLL_Div1;

	// DMA Conf
	bool use_dma = true;
	bool use_dma_irq = false;

	static constexpr DMAConfig default_dma_config = {
		// DMAConfig dma_config = {
		.dir = DMAConfig::Periph2Mem,
		.circular = true,
		.transfer_size_mem = DMAConfig::HalfWord,
		.transfer_size_periph = DMAConfig::HalfWord,
		.dma_priority = DMAConfig::Low,
		.mem_inc = true,
		.periph_inc = false,
		.enable_fifo = false,
	};
	DMAConfig dma_config{default_dma_config};

	bool enable_end_of_sequence_isr = false;
	bool enable_end_of_conversion_isr = false;
};

//adc_peripheral.hh

template<AdcPeriphConfig Conf>
class AdcPeripheral {
public:
	AdcPeripheral() {
		FPin<GPIO::A, 1> p;
		if constexpr (Conf.dma_config.pri == 3)
			p.high();
		else
			p.low();

		if constexpr (Conf.resolution == Bits12) {
			p.high();
			p.low();
		}
	}
};

/////////////////////////
//dma_peripheral.hh
template<DMAConfig Conf>
class DMAPeripheral {
public:
	DMAPeripheral() {

		static_assert(Conf.pri == 3,
					  "Default value of DMAConfig::pri is 3 so it should be detected as such in Testdma::doit()");

		FPin<GPIO::A, 1> p;
		if constexpr (Conf.pri == 3)
			p.high();
		else
			p.low();
	}
};

//////////////////
//main:

static constexpr DMAConfig MyDmaConfig{
	.DMAx = 1,
	.channel = 1,
	//do not override default value of .pri
	.subpri = 2,
};

//use default
static constexpr DMAConfig MyDefaultDMAConfig{};

//override particulars in any order
static constexpr DMAConfig MyModifiedDMAConfig =
	DMAConfig{}.set<&DMAConfig::stream>(3).set<&DMAConfig::circular>(false).set<&DMAConfig::dir>(DMAConfig::Mem2Periph);

//better way to override particular members (but must respect order)
static constexpr DMAConfig MyDMAConfig{
	.DMAx = 2,
	.channel = 2,
	.pri = 2,
	.subpri = 3,
	.dir = DMAConfig::Mem2Periph,
	.circular = true,
};

//nested:

static constexpr AdcPeriphConfig MyDefaultAdcConfig{};
static constexpr AdcPeriphConfig MyDefaultAdcConfig_DefaultDMAFromAdcConfig{.dma_config =
																				AdcPeriphConfig::default_dma_config};
static constexpr AdcPeriphConfig MyDefaultAdcConfig_DefaultDMAFromDMAConfig{
	.dma_config{},
};

//override
static constexpr AdcPeriphConfig MyDefaultAdcConfig_CustomDMA{
	.adc_periph_num = AdcPeriphNum::_2,
	//overrides from DMAConfig's defaults, not AdcConfig::default_dma_config
	.dma_config{
		.DMAx = 3,
		.channel = 4,
	},
};

void setup_some_peripheral() {
	DMAPeripheral<MyDmaConfig> testdma;
	DMAPeripheral<MyModifiedDMAConfig> testdma1;

	AdcPeripheral<MyDefaultAdcConfig> a1;
	AdcPeripheral<MyDefaultAdcConfig_CustomDMA> a2;
	AdcPeripheral<MyDefaultAdcConfig_DefaultDMAFromAdcConfig> a3;
	AdcPeripheral<MyDefaultAdcConfig_DefaultDMAFromDMAConfig> a4;
}

} // namespace mdrivlib
