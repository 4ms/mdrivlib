#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "pin.hh"
#include "rcc.hh"
#include "spi.hh"
#include "system.hh"

// template<unsigned NUM_CHIPS = 1>
// struct SpiPins {
// 	PinNoInit SCLK;
// 	PinNoInit COPI;
// 	PinNoInit CIPO;
// 	PinNoInit CS[NUM_CHIPS];
// };

template<unsigned PERIPH_NUM, unsigned NUM_CHIPS = 1>
struct SpiConfig {
	static inline const unsigned PeriphNum = PERIPH_NUM;
	static inline const unsigned NumChips = NUM_CHIPS;

	IRQType IRQn;

	PinNoInit SCLK;
	PinNoInit COPI;
	PinNoInit CIPO;
	PinNoInit CS[NUM_CHIPS];

	// SpiPins<NUM_CHIPS> pins;

	// unsigned chans_per_chip;
	// uint32_t change_channel_commands[NUM_CHIPS];
	// uint32_t continue_channel_commands[NUM_CHIPS];
};

template<typename ConfT>
class SpiPeriph {
public:
	static inline const uint32_t N = ConfT::PeriphNum;

	SpiPeriph(const ConfT &conf)
		: _conf{conf} {
	}

	void init() {
		disable();

		Pin sclk{_conf.SCLK, PinMode::Alt};
		Pin copi{_conf.COPI, PinMode::Alt};
		Pin cipo{_conf.CIPO, PinMode::Alt};
		for (int i = 0; i < ConfT::NumChips; i++) {
			CS[i].init(_conf.CS[i], PinMode::Output);
			unselect(i);
		}

		target::RCC_Control::SPI<N>::Reg::set();

		// Init SPI here: HAL_SPI_Init();
		SPI_HandleTypeDef spih;
		if constexpr (N == 1)
			spih.Instance = SPI1;
		if constexpr (N == 2)
			spih.Instance = SPI2;
		if constexpr (N == 3)
			spih.Instance = SPI3;
		if constexpr (N == 4)
			spih.Instance = SPI4;
		if constexpr (N == 5)
			spih.Instance = SPI5;
		if constexpr (N == 6)
			spih.Instance = SPI6;
		spih.Init.Mode = SPI_MODE_MASTER;
		spih.Init.Direction = SPI_DIRECTION_2LINES;
		spih.Init.DataSize = SPI_DATASIZE_16BIT;
		spih.Init.CLKPolarity = SPI_POLARITY_LOW;
		spih.Init.CLKPhase = SPI_PHASE_1EDGE;
		spih.Init.NSS = SPI_NSS_SOFT; // SPI_NSS_HARD_OUTPUT;
		spih.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
		spih.Init.FirstBit = SPI_FIRSTBIT_MSB;
		spih.Init.TIMode = SPI_TIMODE_DISABLE;
		spih.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		spih.Init.CRCPolynomial = 7;
		spih.Init.NSSPMode = SPI_NSS_PULSE_DISABLE; // SPI_NSS_PULSE_ENABLE;
		spih.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
		spih.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
		spih.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
		spih.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
		spih.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;			   // right?
		spih.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE; // right?
		spih.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;		   // what is this?
		spih.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;			   // what is this?
		spih.Init.IOSwap = SPI_IO_SWAP_DISABLE;

		HAL_SPI_Init(&spih);

		// if (ConfT::NumChips == 1 && conf.periph_controls_ss)
		//    target::SPI<N>::template CFG2<SPI_CFG2_SSOE>::set();
		//    set SSOM?
		//
		// else
		//    target::SPI<N>::template CFG2<SPI_CFG2_SSOE>::clear();
	}
	void enable_RX_interrupt() {
		target::SPI<N>::template IER<SPI_IER_RXPIE>::set();
	}
	void disable_RX_interrupt() {
		target::SPI<N>::template IER<SPI_IER_RXPIE>::clear();
	}
	void enable_TX_interrupt() {
		target::SPI<N>::template IER<SPI_IER_TXPIE>::set();
	}
	void disable_TX_interrupt() {
		target::SPI<N>::template IER<SPI_IER_TXPIE>::clear();
	}
	void enable_duplex_interrupt() {
		target::SPI<N>::template IER<SPI_IER_DXPIE>::set();
	}
	void disable_duplex_interrupt() {
		target::SPI<N>::template IER<SPI_IER_DXPIE>::clear();
	}
	void enable() {
		target::SPI<N>::template CR1<SPI_CR1_SPE>::set();
	}
	void disable() {
		target::SPI<N>::template CR1<SPI_CR1_SPE>::clear();
	}
	bool rx_packet_available() {
		return target::SPI<N>::template SR<SPI_SR_RXP>::read() ? true : false;
	}
	bool tx_space_available() {
		return target::SPI<N>::template SR<SPI_SR_TXP>::read() ? true : false;
	}
	bool duplex_space_available() {
		return target::SPI<N>::template SR<SPI_SR_DXP>::read() ? true : false;
	}
	void load_to_send(uint16_t data) {
		// Todo: why does default template parameter not work here?
		target::SPI<N>::template TXDR<0xFFFFFFFF>::write(data);
	}
	void send() {
		target::SPI<N>::template CR1<SPI_CR1_CSTART>::set();
	}
	uint16_t received_data() {
		return target::SPI<N>::template RXDR<0x0000FFFF>::read();
	}
	void select(int chip_num) {
		CS[chip_num].low();
	}
	void unselect(int chip_num) {
		CS[chip_num].high();
	}

	// const uint32_t itflag;
	// const uint32_t itsource;

private:
	// SPI_TypeDef *_spi;
	const ConfT &_conf;
	Pin CS[ConfT::NumChips];
};
// end spi.hh

template<typename ConfT>
struct AdcSpi_MAX11666 {
	AdcSpi_MAX11666(const ConfT &conf)
		: periph{conf}
		, cur_chan{0}
		, cur_chip{0} //
	{
		InterruptManager::registerISR(conf.IRQn, [this]() {
			periph.unselect(cur_chip);
			cur_chip = 1 - cur_chip;

			periph.select(cur_chip);

			while (periph.duplex_space_available()) {
				periph.load_to_send(CONTINUE_READING_CH1);
				val[cur_chan] = periph.received_data();
			}
		});

		periph.init();
		periph.enable_duplex_interrupt();
		periph.enable_RX_interrupt();
		periph.enable_TX_interrupt();
		auto pri = System::encode_nvic_priority(0, 1);
		NVIC_SetPriority(conf.IRQn, pri);
		NVIC_EnableIRQ(conf.IRQn);

		periph.enable();
	}
	void start() {
		cur_chip = 0;
		periph.unselect(1 - cur_chip);
		periph.load_to_send(SWITCH_TO_CH1);
		periph.select(cur_chip);
		periph.send();
	}

	uint16_t get_val(int chan) {
		return val[chan];
	}
	void advance_channel() {
		// periph.send(SWITCH_TO_CH1);
	}

private:
	SpiPeriph<ConfT> periph;
	uint16_t val[ConfT::NumChips * 2];
	unsigned cur_chan;
	unsigned cur_chip;

	enum Max11666Commands {
		SWITCH_TO_CH1 = 0x00FF,
		CONTINUE_READING_CH1 = 0x0000,
		SWITCH_TO_CH2 = 0xFF00,
		CONTINUE_READING_CH2 = 0xFFFF,
	};
};

