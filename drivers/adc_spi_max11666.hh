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
	static inline const unsigned N = ConfT::PeriphNum;

	SpiPeriph(const ConfT &conf)
		: _conf{conf} {
		if constexpr (N == 1)
			_spi = SPI1;
		if constexpr (N == 2)
			_spi = SPI2;
		if constexpr (N == 3)
			_spi = SPI3;
		if constexpr (N == 4)
			_spi = SPI4;
		if constexpr (N == 5)
			_spi = SPI5;
		if constexpr (N == 6)
			_spi = SPI6;
	}

	void configure() {
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
		spih.Instance = _spi;
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
		spih.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE; // what is this? Example code says "Recommended"
		spih.Init.IOSwap = SPI_IO_SWAP_DISABLE;

		HAL_SPI_Init(&spih);

		// if (ConfT::NumChips == 1 && conf.periph_controls_ss)
		//    target::SPI<N>::template CFG2<SPI_CFG2_SSOE>::set();
		//    set SSOM?
		//
		// else
		//    target::SPI<N>::template CFG2<SPI_CFG2_SSOE>::clear();
	}

	template<unsigned M>
	using IER = typename target::SPI<N>::template IER<M>;
	template<unsigned M>
	using CR1 = typename target::SPI<N>::template CR1<M>;
	template<unsigned M>
	using CR2 = typename target::SPI<N>::template CR2<M>;
	template<unsigned M>
	using SR = typename target::SPI<N>::template SR<M>;

	void enable_RX_interrupt() {
		IER<SPI_IER_RXPIE>::set();
		// target::SPI<N>::template IER<SPI_IER_RXPIE>::set();
	}
	void disable_RX_interrupt() {
		IER<SPI_IER_RXPIE>::clear();
		// target::SPI<N>::template IER<SPI_IER_RXPIE>::clear();
	}
	void enable_TX_interrupt() {
		IER<SPI_IER_TXPIE>::set();
	}
	void disable_TX_interrupt() {
		IER<SPI_IER_TXPIE>::clear();
	}
	void enable_duplex_interrupt() {
		IER<SPI_IER_DXPIE>::set();
	}
	void disable_duplex_interrupt() {
		IER<SPI_IER_DXPIE>::clear();
	}
	void enable() {
		CR1<SPI_CR1_SPE>::set();
	}
	void disable() {
		CR1<SPI_CR1_SPE>::clear();
	}
	bool rx_packet_available() {
		return SR<SPI_SR_RXP>::read() ? true : false;
	}
	bool tx_space_available() {
		return SR<SPI_SR_TXP>::read() ? true : false;
	}
	bool duplex_space_available() {
		return SR<SPI_SR_DXP>::read() ? true : false;
	}
	bool rx_fifo_not_empty() {
		return SR<SPI_SR_RXWNE>::read() ? true : false;
	}
	int rx_fifo_frames_left() {
		return SR<SPI_SR_RXPLVL>::read() >> SPI_SR_RXPLVL_Pos;
	}
	void set_tx_data_size(uint16_t num_packets) {
		CR2<SPI_CR2_TSIZE>::write(num_packets);
	}
	void load_tx_data(uint16_t data0, uint16_t data1) {
		target::SPI<N>::TXDR::write(data0 << 16 | data1);
	}
	void load_tx_data(uint32_t data) {
		target::SPI<N>::TXDR::write(data);
	}
	void start_transfer() {
		CR1<SPI_CR1_CSTART>::set();
	}
	uint16_t received_data() {
		return target::SPI<N>::RXDR::read();
	}
	void select(int chip_num) {
		CS[chip_num].low();
	}
	void unselect(int chip_num) {
		CS[chip_num].high();
	}

private:
	SPI_TypeDef *_spi;
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

			if (periph.duplex_space_available()) {
				periph.load_tx_data(SWITCH_TO_CH1, CONTINUE_READING_CH1);
				// periph.disable_TX_interrupt()... if we want to stop xfering here.
				store_reading(periph.received_data());
				// periph.disable_RX_interrupt()... if we want to stop xfering here.
			} else if (periph.tx_space_available()) {
				periph.load_tx_data(SWITCH_TO_CH1, CONTINUE_READING_CH1);
			} else if (periph.rx_packet_available()) {
				store_reading(periph.received_data());
			}

			// Todo: maybe we can use the EOT flag to switch chips or channels?
			// if EOT is set, clear it, and clear TXTF and SUSP

			// check for OVR flag, UDR flag, MODF, FRE
		});

		periph.configure();

		auto pri = System::encode_nvic_priority(0, 1);
		NVIC_SetPriority(conf.IRQn, pri);
		NVIC_EnableIRQ(conf.IRQn);
	}
	void start() {
		cur_chip = 1;
		// periph.unselect(1 - cur_chip);
		// periph.select(cur_chip);

		periph.set_tx_data_size(2);
		periph.enable();

		periph.enable_duplex_interrupt();
		periph.enable_RX_interrupt();
		periph.enable_TX_interrupt();

		periph.start_transfer();
	}

	void store_reading(uint16_t reading) {
		val[cur_chip * 2 + cur_chan] = reading;
	}
	void store_reading(int chip, int chan, uint16_t reading) {
		val[chip * 2 + chan] = reading;
	}

	uint16_t get_val(int chan) {
		return val[chan];
	}
	void advance_channel() {
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

