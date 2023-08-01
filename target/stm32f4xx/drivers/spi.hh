#pragma once
#include "drivers/interrupt_control.hh"
#include "drivers/rcc.hh"
#include "drivers/spi_config_struct.hh"
#include "drivers/spi_registers.hh"
#include "util/math.hh"

// Todo: move to target-specific dir (is shared between H7 and MP1, so one can #include the other)
namespace mdrivlib
{

template<typename ConfT>
struct SpiPeriph {
public:
	static inline const unsigned N = ConfT::PeriphNum;

	// Helper shortcuts:
	template<unsigned M>
	using CR1 = typename SPI_<N>::template CR1<M>;

	template<unsigned M>
	using CR2 = typename SPI_<N>::template CR2<M>;

	template<unsigned M>
	using SR = typename SPI_<N>::template SR<M>;

	template<unsigned M>
	using CRCPOLY = typename SPI_<N>::template CRCPOLY<M>;

	SpiPeriph() = default;

	void configure() {
		disable();

		Pin init_sclk{ConfT::SCLK, PinMode::Alt};
		Pin init_copi{ConfT::COPI, PinMode::Alt};
		Pin init_cipo{ConfT::CIPO, PinMode::Alt};

		static_assert(!ConfT::use_hardware_ss ||
						  (ConfT::use_hardware_ss && (ConfT::NumChips == 1) && (ConfT::CS0.af > 0)),
					  "Hardware SS only supported when NumChips = 1, and pin AF > 0");

		if (ConfT::use_hardware_ss) {
			Pin init_nss{ConfT::CS0, PinMode::Alt};
		} else {
			Pin init_cs0{ConfT::CS0, PinMode::Output};
			unselect<0>();
			if constexpr (ConfT::NumChips > 1) {
				Pin init_cs1{ConfT::CS1, PinMode::Output};
				unselect<1>();
			}
			if constexpr (ConfT::NumChips > 2) {
				Pin init_cs2{ConfT::CS2, PinMode::Output};
				unselect<2>();
			}
			if constexpr (ConfT::NumChips > 3) {
				Pin init_cs3{ConfT::CS3, PinMode::Output};
				unselect<3>();
			}
			static_assert(ConfT::NumChips <= 4, "mdrivlib::SpiPeriph only supports selecting 1-4 chips");
		}

		RCC_Enable::SPI<N>::set();

		if constexpr (ConfT::data_dir == SpiDataDir::Duplex) {
			CR1<SPI_CR1_BIDIMODE>::clear(); //2-directional data mode
			CR1<SPI_CR1_RXONLY>::clear();	//Full duplex (rx and tx)
		}
		if constexpr (ConfT::data_dir == SpiDataDir::TXOnly) {
			//See F401 Ref manual sec 20.3.4: "Transmit-only mode is similar to full-duplex mode (BIDIMODE=0, RXONLY=0)"
			CR1<SPI_CR1_BIDIMODE>::clear();
			CR1<SPI_CR1_RXONLY>::clear();
		}
		if constexpr (ConfT::data_dir == SpiDataDir::RXOnly) {
			CR1<SPI_CR1_BIDIMODE>::clear();
			CR1<SPI_CR1_RXONLY>::set();
		}
		if constexpr (ConfT::data_dir == SpiDataDir::HalfDuplex) {
			CR1<SPI_CR1_BIDIMODE>::set();
			//RXONLY?
		}

		set_data_size<ConfT::data_size>();

		if constexpr (ConfT::clock_high_when_idle)
			CR1<SPI_CR1_CPOL>::set();
		else
			CR1<SPI_CR1_CPOL>::clear();

		if constexpr (ConfT::second_clk_transition_captures_data)
			CR1<SPI_CR1_CPHA>::set();
		else
			CR1<SPI_CR1_CPHA>::clear();

		if constexpr (ConfT::use_hardware_ss) {
			CR2<SPI_CR2_SSOE>::set();
			CR1<SPI_CR1_SSM>::clear();
		} else {
			CR2<SPI_CR2_SSOE>::clear();
			CR1<SPI_CR1_SSM>::set();
		}

		if constexpr (!ConfT::use_hardware_ss && ConfT::is_controller)
			CR1<SPI_CR1_SSI>::set();
		else
			CR1<SPI_CR1_SSI>::clear();

		// FixMe: This must be set/cleared AFTER CR1_SSI is set/cleared. Why?
		if constexpr (ConfT::is_controller)
			CR1<SPI_CR1_MSTR>::set();
		else
			CR1<SPI_CR1_MSTR>::clear();

		if constexpr (ConfT::clock_division <= 2)
			CR1<SPI_CR1_BR>::write(0b000);
		else if constexpr (ConfT::clock_division >= 256)
			CR1<SPI_CR1_BR>::write(0b111);
		else
			CR1<SPI_CR1_BR>::write((MathTools::log2_floor(ConfT::clock_division) - 1));

		if constexpr (ConfT::LSBfirst)
			CR1<SPI_CR1_LSBFIRST>::set();
		else
			CR1<SPI_CR1_LSBFIRST>::clear();

		// Todo: make configurable
		// TI Mode
		CR2<SPI_CR2_FRF>::write(0);

		if constexpr (ConfT::EnableCRC) {
			CR1<SPI_CR1_CRCEN>::set();
			CRCPOLY<SPI_CRCPR_CRCPOLY>::write(ConfT::CRCPoly);
		} else
			CR1<SPI_CR1_CRCEN>::clear();
	}

	void enable() {
		CR1<SPI_CR1_SPE>::set();
	}
	void disable() {
		CR1<SPI_CR1_SPE>::clear();
	}

	// clang-format off

	// Interrupts
	void enable_RX_interrupt() { CR2<SPI_CR2_RXNEIE>::set(); }
	void disable_RX_interrupt() { CR2<SPI_CR2_RXNEIE>::clear(); }
	void enable_TX_interrupt() { CR2<SPI_CR2_TXEIE>::set(); }
	void disable_TX_interrupt() { CR2<SPI_CR2_TXEIE>::clear(); }
	void enable_duplex_interrupt() { ; }
	void disable_duplex_interrupt() { ; }
	void enable_end_of_xfer_interrupt() { enable_TX_interrupt(); }
	void disable_end_of_xfer_interrupt() { disable_TX_interrupt(); }

	void enable_error_interrupt() { CR2<SPI_CR2_ERRIE>::set(); }
	void disable_error_interrupt() { CR2<SPI_CR2_ERRIE>::clear(); }
	void enable_TI_mode_frame_error_interrupt() { enable_error_interrupt(); }
	void disable_TI_mode_frame_error_interrupt() { disable_error_interrupt(); }
	void enable_underrun_interrupt() { enable_error_interrupt(); }
	void disable_underrun_interrupt() { disable_error_interrupt(); }
	void enable_overrun_interrupt() { enable_error_interrupt(); }
	void disable_overrun_interrupt() { disable_error_interrupt(); }
	void enable_mode_fault_interrupt() { enable_error_interrupt(); }
	void disable_mode_fault_interrupt() { disable_error_interrupt(); }
	void enable_crc_error_interrupt() { enable_error_interrupt(); }
	void disable_crc_error_interrupt() { disable_error_interrupt(); }

	void enable_tx_filled_interrupt() { enable_TX_interrupt(); }
	void disable_tx_filled_interrupt() { disable_TX_interrupt(); }
	void enable_tx_reload_interrupt() { }
	void disable_tx_reload_interrupt() { }

	void clear_EOT_flag() { }
	void clear_TXTF_flag() { }
	void clear_end_of_xfer_flag() { }
	void clear_TI_mode_frame_error_flag() { }
	void clear_overrun_flag() { }
	void clear_underrun_flag() { }
	void clear_mode_fault_flag() { }
	void clear_crc_error_flag() { }
	void clear_tx_filled_flag() { }
	void clear_tx_reload_flag() { }

	// Status flags
	bool rx_packet_available() { return false; }
	bool tx_space_available() { return false; }
	bool duplex_space_available() { return false; }
	bool is_end_of_transfer() { return SR<SPI_SR_BSY>::read() ? false : true; }
	bool is_tx_complete() { return SR<SPI_SR_BSY>::read() ? false : true; }
	bool is_overrun() { return SR<SPI_SR_OVR>::read() ? true : false; }
	bool is_underrun() { return SR<SPI_SR_UDR>::read() ? true : false; }
	bool rx_fifo_not_empty() { return false; }
	int rx_fifo_frames_left() { return false; }

	// clang-format off

	void enable_tx_dma() { CR2<SPI_CR2_TXDMAEN>::set(); }
	uint32_t get_tx_datareg_addr() { return reinterpret_cast<uint32_t>(SPI_<N>::TXDR::BaseAddress); }

	// TX conditions
	void set_tx_message_size(uint16_t num_packets) {
	}
	void set_tx_message_reload_size(uint16_t num_packets) {
	}
	void set_data_size() {
		CR1<SPI_CR1_DFF>::write(ConfT::data_size == 16 ? 1 : 0);
	}
	void set_fifo_threshold(uint8_t num_bytes) {
	}
	void load_tx_data(uint16_t data0, uint16_t data1) {
		SPI_<N>::TXDR::write(data0 << 16 | data1);
	}
	void load_tx_data(uint32_t data) {
		SPI_<N>::DR::write(data);
	}
	void start_transfer() {
		//starts on load_tx_data()
	}
	uint16_t received_data() {
		return SPI_<N>::DR::read();
	}

	// Half-duplex mode
	void start_half_duplex_tx() {
		if constexpr (ConfT::data_dir == SpiDataDir::HalfDuplex) {
			CR1<SPI_CR1_BIDIOE>::set();
		}
	}
	void start_half_duplex_rx() {
		if constexpr (ConfT::data_dir == SpiDataDir::HalfDuplex) {
			CR1<SPI_CR1_BIDIOE>::clear();
		}
	}

	void enable_auto_suspend() {
	}

	template<int chip_num>
	void select() {
		if constexpr (chip_num == 0)
			FPin<ConfT::CS0.gpio, ConfT::CS0.pin, PinMode::Output>::low();
		if constexpr (chip_num == 1)
			FPin<ConfT::CS1.gpio, ConfT::CS1.pin, PinMode::Output>::low();
		if constexpr (chip_num == 2)
			FPin<ConfT::CS2.gpio, ConfT::CS2.pin, PinMode::Output>::low();
		if constexpr (chip_num == 3)
			FPin<ConfT::CS3.gpio, ConfT::CS3.pin, PinMode::Output>::low();
	}
	template<int chip_num>
	void unselect() {
		if constexpr (chip_num == 0)
			FPin<ConfT::CS0.gpio, ConfT::CS0.pin, PinMode::Output>::high();
		if constexpr (chip_num == 1)
			FPin<ConfT::CS1.gpio, ConfT::CS1.pin, PinMode::Output>::high();
		if constexpr (chip_num == 2)
			FPin<ConfT::CS2.gpio, ConfT::CS2.pin, PinMode::Output>::high();
		if constexpr (chip_num == 3)
			FPin<ConfT::CS3.gpio, ConfT::CS3.pin, PinMode::Output>::high();
	}
	void select(int chip_num) {
		if (chip_num == 0)
			select<0>();
		if (chip_num == 1)
			select<1>();
		if (chip_num == 2)
			select<2>();
		if (chip_num == 3)
			select<3>();
	}
	void unselect(int chip_num) {
		if (chip_num == 0)
			unselect<0>();
		if (chip_num == 1)
			unselect<1>();
		if (chip_num == 2)
			unselect<2>();
		if (chip_num == 3)
			unselect<3>();
	}

private:
	FPin<ConfT::CS0.gpio, ConfT::CS0.pin> CS0;
	FPin<ConfT::CS1.gpio, ConfT::CS1.pin> CS1;
	FPin<ConfT::CS2.gpio, ConfT::CS2.pin> CS2;
	FPin<ConfT::CS3.gpio, ConfT::CS3.pin> CS3;
};
} // namespace mdrivlib
