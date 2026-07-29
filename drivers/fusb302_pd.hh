#pragma once
// Minimal USB-PD 2.0 sink policy engine for the FUSB302.
//
// Purpose-built for one job: land the implicit 5V contract a PD source
// requires, then get the *data* roles swapped (DR_Swap) so a partner like the
// OXI One in "Device Self Powered" mode -- power source but data device, with
// its D+ pull-up gated on a PD contract + data-role swap -- will finally
// present D+ and enumerate. We never source power and never do PR_Swap: power
// stays partner->us (we ignore it; the module is Eurorack-powered), only the
// data role moves.
//
// Leans on the FUSB302 hardware assists: auto-GoodCRC (with role/rev bits from
// Switches1), auto-retry on missing GoodCRC, hard-reset signaling. This engine
// only builds/parses the policy-level messages.
//
// NOT a general PD stack: SOP only (no cables/VCONN), Rev 2.0 header (sources
// downgrade), fixed 5V request, no PPS/extended/chunked messages.
//
// Include from fusb302.hh (after its pr_debug definition).

#include "drivers/fusb302_registers.hh"
#include <cstdint>

namespace FUSB302
{

template<typename Dev>
struct PDSink {

	enum class State : uint8_t {
		Disabled,	// not attached as sink, or PD given up
		WaitCaps,	// attached, waiting for Source_Capabilities
		WaitAccept, // Request sent, waiting for Accept
		WaitPSRdy,	// Accepted, waiting for PS_RDY
		Ready,		// explicit contract established
		SwapSent,	// we sent DR_Swap, waiting for Accept/Reject
	};

	Dev &dev;

	State state = State::Disabled;
	bool data_role_host = false;  // true once a DR_Swap made us the DFP
	bool swap_pending_tx = false; // we accepted partner's DR_Swap; swap on TxSent
	bool swap_refused = false;	  // partner rejected our DR_Swap (don't retry)
	bool partner_is_pd = false;	  // saw at least one good packet from partner
	uint8_t tx_msg_id = 0;
	int8_t last_rx_msg_id = -1;
	uint32_t enabled_tick = 0;
	uint32_t state_tick = 0;

	static constexpr uint32_t CapsGraceMs = 1000;	  // no caps by then => not a PD source
	static constexpr uint32_t ResponseTimeoutMs = 250; // covers tSenderResponse+margin
	static constexpr uint32_t FifoAddr = 0x43;

	// PD 2.0 control message types
	enum Ctrl : uint8_t {
		GoodCRC = 0x1,
		GotoMin = 0x2,
		Accept = 0x3,
		Reject = 0x4,
		Ping = 0x5,
		PS_RDY = 0x6,
		Get_Source_Cap = 0x7,
		Get_Sink_Cap = 0x8,
		DR_Swap = 0x9,
		PR_Swap = 0xA,
		VCONN_Swap = 0xB,
		Wait = 0xC,
		Soft_Reset = 0xD,
	};
	// PD 2.0 data message types
	enum Data : uint8_t {
		Source_Capabilities = 0x1,
		Request = 0x2,
		BIST = 0x3,
		Sink_Capabilities = 0x4,
		Vendor_Defined = 0xF,
	};

	// FUSB302 TX FIFO tokens
	enum Token : uint8_t {
		TXON = 0xA1,
		SYNC1 = 0x12,
		SYNC2 = 0x13,
		PACKSYM = 0x80,
		JAM_CRC = 0xFF,
		EOP = 0x14,
		TXOFF = 0xFE,
	};

	PDSink(Dev &dev)
		: dev{dev} {
	}

	// Call once attached as sink, with the live CC already selected for
	// measurement in Switches0 (this engine transmits and auto-GoodCRCs on
	// that same CC).
	void enable(bool cc_is_cc2) {
		// BMC TX on the live CC; auto-GoodCRC with sink/UFP roles, Rev 2.0
		dev.template write<Switches1>({.EnableTXCC1 = uint8_t(cc_is_cc2 ? 0 : 1),
									   .EnableTXCC2 = uint8_t(cc_is_cc2 ? 1 : 0),
									   .AutoCRC = 1,
									   .DataRoleSrc = 0,
									   .SpecRev = 1,
									   .PowerRoleSrc = 0});
		// Hardware retries on missing GoodCRC. (Raw write: Control3 derives
		// from both ReadWrite and WriteClear, which makes the typed write_reg
		// constraint ambiguous.)
		write_raw(Control3::Address, 0x07); // AutoRetryCRC | NumRetries=3
		// All power blocks on (IntOsc is required for BMC TX)
		dev.template write<Power>({.BandGapAndWake = 1, .MeasureBlock = 1, .RXAndCurrentRefs = 1, .IntOsc = 1});
		// Unmask the PD interrupts (chip-level Mask for CRC_CHK is handled by
		// the caller's AsDevice mask write)
		dev.template write<MaskA>({.HardResetRx = 0,
								   .SoftResetRx = 0,
								   .TxSent = 0,
								   .HardResetSent = 1,
								   .RetryFail = 0,
								   .SoftFail = 1,
								   .ToggleDone = 0,
								   .OCPTempEvent = 1});
		// Clean PD block + FIFOs
		dev.template write<Reset>({.SWReset = 0, .PDReset = 1});
		flush_fifos();

		state = State::WaitCaps;
		data_role_host = false;
		swap_pending_tx = false;
		swap_refused = false;
		partner_is_pd = false;
		tx_msg_id = 0;
		last_rx_msg_id = -1;
		enabled_tick = state_tick = HAL_GetTick();
		pr_debug("PD: sink engine enabled on CC%d\n", cc_is_cc2 ? 2 : 1);
	}

	// Call whenever the chip is reset / we detach (state only; the chip's PD
	// block is wiped by SWReset anyway)
	void on_chip_reset() {
		state = State::Disabled;
		data_role_host = false;
		swap_pending_tx = false;
	}

	bool active() const {
		return state != State::Disabled;
	}

	// Contract established (or in progress past caps): the partner speaks PD
	bool partner_pd() const {
		return partner_is_pd;
	}

	bool contract_ready() const {
		return state == State::Ready || state == State::SwapSent;
	}

	bool data_role_is_host() const {
		return data_role_host;
	}

	// True while the engine is mid-negotiation and the caller should hold off
	// other (blind) role experiments. Deliberately NOT true while merely
	// waiting for Source_Capabilities from a partner that has never spoken PD:
	// most partners are not PD sources, and delaying the role fallback for
	// them breaks partners with narrow attach windows (the OXI One only
	// presents D+ for ~3s after attach -- a 1s hold-off here pushed the host
	// trial past it). A real PD source's caps arrive well inside the fallback
	// timeout and move the state on anyway.
	bool busy() const {
		switch (state) {
			case State::WaitCaps:
				return partner_is_pd && HAL_GetTick() - enabled_tick < CapsGraceMs;
			case State::WaitAccept:
			case State::WaitPSRdy:
			case State::SwapSent:
				return true;
			default:
				return false;
		}
	}

	// Ask the partner to take the UFP (device) data role, making us the DFP.
	// Caller decides the policy (e.g. only when not enumerated as a device).
	// Returns true if the request was sent.
	bool request_dr_swap() {
		if (state != State::Ready || data_role_host || swap_refused)
			return false;
		pr_debug("PD: sending DR_Swap\n");
		send_control(Ctrl::DR_Swap);
		state = State::SwapSent;
		state_tick = HAL_GetTick();
		return true;
	}

	// Service the engine. intra must be the InterruptA flags just read (and
	// thereby cleared) by the caller's interrupt handler.
	void service(InterruptA intra) {
		if (state == State::Disabled)
			return;

		if (intra.HardResetRx) {
			// Source hard reset: VBUS will cycle; PD restarts from caps.
			// (The caller's link-down handling may also tear the whole
			// attach down, which is fine too.)
			pr_debug("PD: hard reset received\n");
			dev.template write<Reset>({.SWReset = 0, .PDReset = 1});
			flush_fifos();
			state = State::WaitCaps;
			swap_pending_tx = false;
			data_role_host = false;
			tx_msg_id = 0;
			last_rx_msg_id = -1;
			enabled_tick = state_tick = HAL_GetTick();
			return;
		}

		if (intra.SoftResetRx) {
			pr_debug("PD: soft reset received\n");
			tx_msg_id = 0;
			last_rx_msg_id = -1;
			send_control(Ctrl::Accept);
			state = State::WaitCaps; // source re-sends caps next
			state_tick = HAL_GetTick();
		}

		if (intra.TxSent) {
			tx_msg_id = (tx_msg_id + 1) & 7;
			if (swap_pending_tx) {
				// Our Accept of the partner's DR_Swap is delivered: swap
				swap_pending_tx = false;
				become_dfp();
			}
		}

		if (intra.RetryFail) {
			// Partner never GoodCRC'd us: treat as not-PD / lost
			pr_debug("PD: retries failed, disabling\n");
			state = State::Disabled;
			return;
		}

		drain_rx_fifo();
		check_timeouts();
	}

	// Periodic poll (call every ~50ms while attached as sink): advances
	// timeouts and catches any RX the level interrupt path missed. INT_N-driven
	// service() handles the fast paths; this is the safety net.
	void tick() {
		if (state == State::Disabled)
			return;
		drain_rx_fifo();
		check_timeouts();
	}

private:
	void become_dfp() {
		data_role_host = true;
		state = State::Ready;
		// Future auto-GoodCRCs must carry the DFP data role bit
		auto sw1 = dev.template read<Switches1>();
		sw1.DataRoleSrc = 1;
		dev.template write<Switches1>(sw1);
		pr_debug("PD: data role swapped, we are now DFP (host)\n");
	}

	void check_timeouts() {
		auto now = HAL_GetTick();
		switch (state) {
			case State::WaitAccept:
			case State::WaitPSRdy:
				if (now - state_tick > ResponseTimeoutMs) {
					pr_debug("PD: contract timeout in state %d\n", (int)state);
					state = State::Disabled;
				}
				break;
			case State::SwapSent:
				if (now - state_tick > ResponseTimeoutMs) {
					pr_debug("PD: DR_Swap not answered\n");
					swap_refused = true;
					state = State::Ready;
				}
				break;
			default:
				break;
		}
	}

	void write_raw(uint8_t reg_addr, uint8_t val) {
		dev.i2c.mem_write(dev.dev_addr, reg_addr, 1, &val, 1);
	}

	void flush_rx() {
		write_raw(Control1::Address, 0x04); // RXFlush (see Control3 note)
	}

	void flush_fifos() {
		dev.template write<Control0WC>({.TXStart = 0, .TXFlush = 1});
		flush_rx();
	}

	// ---- TX ----

	uint16_t make_header(uint8_t type, uint8_t num_objects) {
		// data role and power role bits reflect our current roles
		return uint16_t(type) | (uint16_t(data_role_host ? 1 : 0) << 5) | (uint16_t(1) << 6) /* Rev 2.0 */ |
			   (uint16_t(0) << 8) /* sink */ | (uint16_t(tx_msg_id) << 9) | (uint16_t(num_objects) << 12);
	}

	void send_control(uint8_t type) {
		send_message(make_header(type, 0), nullptr, 0);
	}

	void send_data(uint8_t type, const uint32_t *objs, uint8_t num) {
		send_message(make_header(type, num), objs, num);
	}

	void send_message(uint16_t header, const uint32_t *objs, uint8_t num_objects) {
		uint8_t buf[4 + 1 + 2 + 7 * 4 + 3];
		unsigned n = 0;
		buf[n++] = Token::SYNC1;
		buf[n++] = Token::SYNC1;
		buf[n++] = Token::SYNC1;
		buf[n++] = Token::SYNC2;
		buf[n++] = Token::PACKSYM | uint8_t(2 + 4 * num_objects);
		buf[n++] = header & 0xFF;
		buf[n++] = header >> 8;
		for (unsigned i = 0; i < num_objects; i++) {
			buf[n++] = objs[i] & 0xFF;
			buf[n++] = (objs[i] >> 8) & 0xFF;
			buf[n++] = (objs[i] >> 16) & 0xFF;
			buf[n++] = (objs[i] >> 24) & 0xFF;
		}
		buf[n++] = Token::JAM_CRC;
		buf[n++] = Token::EOP;
		buf[n++] = Token::TXOFF;

		dev.template write<Control0WC>({.TXStart = 0, .TXFlush = 1});
		dev.i2c.mem_write(dev.dev_addr, FifoAddr, 1, buf, n);
		dev.template write<Control0WC>({.TXStart = 1, .TXFlush = 0});
	}

	// ---- RX ----

	void drain_rx_fifo() {
		// Bounded: a burst of packets can't wedge the main loop
		for (int i = 0; i < 4; i++) {
			auto status1 = dev.template read<Status1>();
			if (status1.RXEmpty)
				return;

			uint8_t token{};
			dev.i2c.mem_read(dev.dev_addr, FifoAddr, 1, &token, 1);
			if ((token & 0xE0) != 0xE0) {
				// Not an SOP packet (SOP'/'', or FIFO out of sync): discard all
				pr_debug("PD: non-SOP token 0x%x, flushing RX\n", token);
				flush_rx();
				return;
			}

			uint8_t hdr[2];
			dev.i2c.mem_read(dev.dev_addr, FifoAddr, 1, hdr, 2);
			uint16_t header = uint16_t(hdr[0]) | (uint16_t(hdr[1]) << 8);
			uint8_t num_objects = (header >> 12) & 0x7;

			// Objects + the 4 CRC bytes the chip appends (CRC already checked)
			uint8_t payload[7 * 4 + 4];
			dev.i2c.mem_read(dev.dev_addr, FifoAddr, 1, payload, 4 * num_objects + 4);

			uint32_t objs[7];
			for (unsigned o = 0; o < num_objects; o++)
				objs[o] = uint32_t(payload[4 * o]) | (uint32_t(payload[4 * o + 1]) << 8) |
						  (uint32_t(payload[4 * o + 2]) << 16) | (uint32_t(payload[4 * o + 3]) << 24);

			handle_message(header, objs, num_objects);
		}
	}

	void handle_message(uint16_t header, const uint32_t *objs, uint8_t num_objects) {
		uint8_t type = header & 0x1F;
		int8_t msg_id = (header >> 9) & 0x7;

		if (num_objects == 0 && type == Ctrl::GoodCRC)
			return; // partner's ack of our TX; progress comes via TxSent

		partner_is_pd = true;

		// Drop retransmissions (partner didn't see our GoodCRC)
		if (msg_id == last_rx_msg_id)
			return;
		last_rx_msg_id = msg_id;

		if (num_objects > 0)
			handle_data_msg(type, objs, num_objects);
		else
			handle_control_msg(type);
	}

	void handle_data_msg(uint8_t type, const uint32_t *objs, uint8_t num_objects) {
		switch (type) {
			case Data::Source_Capabilities: {
				// Always request object 1: the spec guarantees PDO 1 is
				// vSafe5V fixed. We draw (almost) nothing: 100mA op/max.
				pr_debug("PD: got source caps (%d PDOs, PDO1=0x%08x)\n", num_objects, (unsigned)objs[0]);
				uint32_t rdo = (1u << 28)	  // object position 1
							 | (1u << 25)	  // USB communications capable
							 | (1u << 24)	  // no USB suspend
							 | (10u << 10)	  // operating current 100mA
							 | (10u << 0);	  // max operating current 100mA
				send_data(Data::Request, &rdo, 1);
				state = State::WaitAccept;
				state_tick = HAL_GetTick();
			} break;

			case Data::Vendor_Defined:
			case Data::BIST:
			default:
				// GoodCRC (auto) is all these get
				break;
		}
	}

	void handle_control_msg(uint8_t type) {
		switch (type) {
			case Ctrl::Accept:
				if (state == State::WaitAccept) {
					state = State::WaitPSRdy;
					state_tick = HAL_GetTick();
				} else if (state == State::SwapSent) {
					// Partner accepted our DR_Swap
					become_dfp();
				}
				break;

			case Ctrl::PS_RDY:
				if (state == State::WaitPSRdy) {
					pr_debug("PD: explicit contract established\n");
					state = State::Ready;
					state_tick = HAL_GetTick();
				}
				break;

			case Ctrl::Reject:
			case Ctrl::Wait:
				if (state == State::SwapSent) {
					pr_debug("PD: DR_Swap refused\n");
					swap_refused = true;
					state = State::Ready;
				} else if (state == State::WaitAccept) {
					// 5V/100mA refused: nothing more we can do
					state = State::Disabled;
				}
				break;

			case Ctrl::DR_Swap:
				// Partner wants to swap data roles. If we're still UFP,
				// that makes us the host: exactly what we want. (If we're
				// already DFP the swap would demote us; refuse.)
				if (!data_role_host && contract_ready()) {
					pr_debug("PD: partner requests DR_Swap, accepting\n");
					send_control(Ctrl::Accept);
					swap_pending_tx = true; // swap when Accept is delivered
				} else {
					send_control(Ctrl::Reject);
				}
				break;

			case Ctrl::PR_Swap:	   // we never take the power-source role
			case Ctrl::VCONN_Swap: // we never supply VCONN
			case Ctrl::Get_Source_Cap:
				send_control(Ctrl::Reject);
				break;

			case Ctrl::Get_Sink_Cap: {
				// Fixed 5V 100mA sink; unconstrained power (Eurorack-powered),
				// USB comm capable, dual-role data
				uint32_t pdo = (1u << 27)	 // unconstrained power
							 | (1u << 26)	 // USB communications capable
							 | (1u << 25)	 // dual-role data
							 | (100u << 10)	 // 5V in 50mV units
							 | (10u << 0);	 // 100mA in 10mA units
				send_data(Data::Sink_Capabilities, &pdo, 1);
			} break;

			case Ctrl::Soft_Reset:
				tx_msg_id = 0;
				last_rx_msg_id = -1;
				send_control(Ctrl::Accept);
				state = State::WaitCaps;
				state_tick = HAL_GetTick();
				break;

			case Ctrl::GotoMin:
			case Ctrl::Ping:
			default:
				break;
		}
	}
};

} // namespace FUSB302
