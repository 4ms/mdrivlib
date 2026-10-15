#pragma once
#include "drivers/fusb302_pd.hh"
#include "drivers/i2c.hh"
#include "fusb302_registers.hh"
#include <cstdint>

// Debugging:
// #define FUSBDEBUG

#ifdef FUSBDEBUG
#include <cstdio>
#define pr_debug printf
#include <optional>
#include <string_view>
#else
static inline void pr_debug(...) {
}
#endif

// A large portion of this driver is for handling some non-compliant USB devices.
// Example: OXI One mkI in "Device Self Powered" mode (only that mode, not in Device or Host mode).
//
// Symptom: a connecting unit holds VBUS high the entire time it's connected (like a Host would do)
// and also toggles an Rp (pull-up) on/off on the CC lines at around 80ms high + 40ms low period.
// However, it never enumerates attached devices and only wants to be a device itself.
// Despite CC toggling, it continues to operate as a Device normally.
//
// Problem 1: Normally if we see an attached unit present VBUS then we assume it's a
// Host since that's what the USB standard dictates. However in this case we need to treat it as a device.
//
// Problem 2: CC toggling can indicate a disconnect when it goes low. It also can confuse
// the state machine because toggling Rp/Rd indicates the attached unit can be host or device,
// but in this case it only wants to be a Device.
//
// Detection of problem units:
// When we plug into a device like this, we see VBUS high from the partner,
// so we present a Rd (advertise ourselves as device/sink).
// If the other machine doesn't enumerate us, then we try another way.
//
// We first try to detect the Rp/no-Rp toggling. We keep our Rd presented and sample the CC lines
// over a 120ms period to see if the BC Levels are toggling. If we see toggling, steady high
// VBUS, and no response when we try present as a device (presenting Rd), then we've found
// a problem unit and proceed with the remedy.
//
// Sometimes we detect the toggling by chance: first we see VBUS high and an Rp, so we start
// to attach as a device. Then we see BCLevel drop to 0 while VBUS is still high. This means
// the partner dropped its Rp (toggled) and is now advertising as a device, yet it keeps
// sourcing VBUS.
//
// Remedy: To connect to such a unit, we present an Rp to advertise as a host,
// and ignore all CC toggles. We do not supply VBUS since it's already present.
//

namespace FUSB302
{

struct Device {

	mdrivlib::I2CPeriph &i2c;
	uint8_t dev_addr;
	uint8_t device_id = 0;

	enum class ConnectedState { None, TogglePolling, AsHost, AsDevice } state = ConnectedState::None;
	uint8_t last_polling_mode = Control2::PollDRP;

	// The OXI One in Device Self Powered mode toggles the CC lines even
	// when connected. We don't want to interpret that as a disconnect (link down).
	// So, we enable a debouncer once a connection is established.
	bool link_down_debounce_enabled = false;
	uint32_t link_down_since = 0;
	uint32_t link_down_last_seen = 0;
	static constexpr uint32_t LinkDownDebounceMs = 350;
	static constexpr uint32_t LinkDownEpisodeMs = 600; // > UsbManager's 250ms link poll period

	void set_link_debounce(bool enabled) {
		link_down_debounce_enabled = enabled;
		if (!enabled)
			link_down_since = 0;
	}

	// Set when attached AsHost to a partner device that sources VBUS itself
	// (self-powered DRP like the OXI One, or a backfeeding gadget rig). For
	// these partners VBUS-loss is the only trustworthy detach signal since
	// the CC lines are toggled during normal operation.
	bool host_partner_sources_vbus = false;

	// Minimal PD sink engine (contract + DR_Swap) in fusb302_pd.hh.
	// FIXME: Not tested on any external PD device!
	PDSink<Device> pd{*this};

	Device(mdrivlib::I2CPeriph &i2c, uint8_t device_addr)
		: i2c{i2c}
		, dev_addr{device_addr} {
	}

	bool init() {
		uint8_t data;
		data = read<ID>();
		if (!data)
			return false;

		device_id = data;

		// Read Interrupt registers to clear them
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();
		return true;
	}

	uint8_t get_chip_id() {
		return device_id;
	}

	// Software-reset the FUSB302: all registers return to their power-on defaults
	// and any pending interrupts are cleared.
	void reset() {
		write<Reset>({.SWReset = 1});
		HAL_Delay(10); // let the POR settle before further register access
		// Interrupt registers are cleared on read
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();
	}

	// Auto host/device: DRP toggle, settling to either role depending on what's
	// attached
	void start_drp_polling() {
		start_toggle_polling(Control2::PollDRP);
	}

	// Force host role: SRC-only toggle. The port presents Rp (pull-up) and only
	// ever attaches as a host (to a device/sink). Will not attach to another host.
	void start_src_polling() {
		start_toggle_polling(Control2::PollSRC);
	}

	// Force device role: SNK-only toggle. The port presents Rd (pull-down) and
	// only ever attaches as a device (to a host/source).
	void start_snk_polling() {
		start_toggle_polling(Control2::PollSNK);
	}

	// probe_snk_for_device():
	// Used when we are in Device-only mode to detect if the user plugged in a device.
	// The device is ignored (as it should be -- the user explicitly set a preference
	// to NOT act as a Host), but this could be confusing for the user if they
	// forgot or didn't understand the preference they set.
	// Solution is to periodically poll for attached devices when we are in unattached
	// Device-only mode. If we see one, we can notify the user that they need to change
	// to Auto or Host-only mode to use the attached device.
	//
	// How it works: When in SNK polling (Device-only) we momentarily present an Rp
	// and read the CC lines to see if there is an Rd attached. We don't source VBUS
	// (or VCONN) so the downstream device doesn't get powered or enumerated.
	// Then we restore SNK-only polling.
	bool probe_snk_for_device() {
		// Stop the toggle state machine so it can neither fire I_TOGGLE nor drive
		// the CC pins while we measure them manually.
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});

		// Power up for a real CC measurement
		write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 1, .MeasuringBlock = 1, .IntOsc = 0});
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 0});

		bool device_present = false;
		// Try CC1 and then CC2
		for (uint8_t cc_n = 0; cc_n <= 1 && !device_present; cc_n++) {
			write<Switches0>(
				{.MeasureCC1 = uint8_t(cc_n == 0), .MeasureCC2 = uint8_t(cc_n == 1), .PullUpCC1 = 1, .PullUpCC2 = 1});
			HAL_Delay(2); // let the BC_LVL comparator settle
			Status0 probe{read<Status0>()};
			pr_debug("Device probe: CC%d BCLevel=%d\n", cc_n ? 2 : 1, probe.BCLevel);
			// With Rp presented, BC_LVL reads the partner's pull-down: 3 (0b11) is
			// an open line (no device), 0 is Ra/VCONN only (a powered cable, not a
			// device); 1 or 2 means a device's Rd is pulling CC into range.
			if (probe.BCLevel == 1 || probe.BCLevel == 2)
				device_present = true;
		}

		// Restore SNK toggle polling: drop the manual Rp, power the measure block
		// back down (the toggle SM manages it), clear any interrupt the manual CC
		// changes latched, and re-enable the SNK toggle. The toggle bit has been
		// off throughout the measurement (well over the off->on settle time), so
		// no extra delay is needed before re-enabling it.
		write<Switches0>({.ConnectVConnCC1 = 0, .ConnectVConnCC2 = 0});
		write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 0, .MeasuringBlock = 0, .IntOsc = 0});
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();
		write<Control2>({.Toggle = 1, .PollingMode = Control2::PollSNK, .ToggleIgnoreRa = 1});

		return device_present;
	}

	// Start the FUSB302 toggle state machine in the given polling mode (Control2
	// PollDRP/PollSRC/PollSNK). All three roles use the same attach/detach
	// detection: on settle, I_TOGGLE fires and Status1A.TOGSS reports the outcome
	// (decoded in handle_interrupt). Only the set of roles toggled differs.
	void start_toggle_polling(uint8_t polling_mode) {
		last_polling_mode = polling_mode;
		set_link_debounce(false); // re-enabled by the manager once enumerated
		host_partner_sources_vbus = false;
		pd.on_chip_reset(); // SWReset below wipes the chip's PD block
		reset();

		// Setup per datasheet p. 7 (Toggle Functionality)
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 0});

		// Clear interrupts
		read<Interrupt>();
		read<InterruptA>();
		read<InterruptB>();

		// Turn toggle bit off, then on (otherwise TOGGLE mode is not re-started)
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		HAL_Delay(10);
		write<Control2>({.Toggle = 1, .PollingMode = polling_mode, .ToggleIgnoreRa = 1});

		write<Switches0>({.ConnectVConnCC1 = 0, .ConnectVConnCC2 = 0});
		// Note: setting Mask::VBusOK to 0 when HostCurrentReq is 0 results in it
		// not detecting disconnect as host
		write<Mask>({.HostCurrentReq = 0,
					 .Collision = 1,
					 .Wake = 1,
					 .Alert = 1,
					 .CRCCheck = 1,
					 .CompChange = 1,
					 .CCBusActivity = 1,
					 .VBusOK = 1});
		write<MaskA>({.HardResetRx = 1,
					  .SoftResetRx = 1,
					  .TxSent = 1,
					  .HardResetSent = 1,
					  .RetryFail = 1,
					  .SoftFail = 1,
					  .ToggleDone = 0,
					  .OCPTempEvent = 1});
		write<MaskB>({.GoodCRCSent = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 0, .MeasuringBlock = 0, .IntOsc = 0});

		dump_all_regs();

		state = ConnectedState::TogglePolling;
	}

	ConnectedState get_state() {
		return state;
	}

	// Force attachment as a Host to a "problem unit" partner that supplies VBUS and also
	// toggles CC (e.g. OXI One in Device Self Powered mode, see comments at top).
	void attach_as_static_src(bool meas_cc2) {
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 1, .MeasuringBlock = 1, .IntOsc = 0});
		write<Switches0>({.MeasureCC1 = uint8_t(meas_cc2 ? 0 : 1),
						  .MeasureCC2 = uint8_t(meas_cc2 ? 1 : 0),
						  .PullUpCC1 = 1,
						  .PullUpCC2 = 1});
		// The partner's CC toggling would trigger BC_LVL/COMP interrupts, so mask them.
		// Unmask VBusOK (rise is reliable, and loss is checked by the USB manager polling)
		write<Mask>({.HostCurrentReq = 1,
					 .Collision = 1,
					 .Wake = 1,
					 .Alert = 1,
					 .CRCCheck = 1,
					 .CompChange = 1,
					 .CCBusActivity = 1,
					 .VBusOK = 0});
		host_partner_sources_vbus = true;
		pd.on_chip_reset(); // partner classified non-PD; stop servicing
		state = ConnectedState::AsHost;
	}

	// --- Characterization/experiment support ---

	// Present Rd on both CC pins, statically: no toggle state machine, no
	// interrupts (caller polls), measure block powered. The partner sees a
	// plain attached sink and nothing we do ever perturbs the CC lines.
	void configure_static_sink() {
		reset();
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 1});
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 1, .MeasuringBlock = 1, .IntOsc = 0});
		write<Switches0>({.PullDownCC1 = 1, .PullDownCC2 = 1, .MeasureCC1 = 1, .MeasureCC2 = 0});
		// Everything masked; the experiment loop polls Status0
		write<Mask>(Mask::make(0xFF));
		write<MaskA>(MaskA::make(0xFF));
		write<MaskB>(MaskB::make(0x01));
		state = ConnectedState::AsDevice;
	}

	// Present Rp on both CC pins, statically (source persona): no toggle, no
	// interrupts, measure block powered. BC_LVL semantics with Rp presented:
	// 3 = open, 0 = Ra, 1/2 = partner Rd.
	void configure_static_src() {
		reset();
		write<Control0>({.HostCurrent = Control0::DefaultCurrent, .MaskAllInt = 1});
		write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
		write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 1, .MeasuringBlock = 1, .IntOsc = 0});
		write<Switches0>({.MeasureCC1 = 1, .MeasureCC2 = 0, .PullUpCC1 = 1, .PullUpCC2 = 1});
		write<Mask>(Mask::make(0xFF));
		write<MaskA>(MaskA::make(0xFF));
		write<MaskB>(MaskB::make(0x01));
		state = ConnectedState::AsHost;
	}

	struct CCLevels {
		uint8_t cc1;
		uint8_t cc2;
		uint8_t vbusok;
	};

	// Measure both CC lines (BC_LVL) and VBUS. Only the internal measurement
	// mux is switched. What we present on the pins is untouched.
	CCLevels read_both_cc() {
		auto sw = read<Switches0>();
		sw.MeasureCC1 = 1;
		sw.MeasureCC2 = 0;
		write<Switches0>(sw);
		HAL_Delay(1);
		Status0 s1{read<Status0>()};
		sw.MeasureCC1 = 0;
		sw.MeasureCC2 = 1;
		write<Switches0>(sw);
		HAL_Delay(1);
		Status0 s2{read<Status0>()};
		return {s1.BCLevel, s2.BCLevel, s2.VBusOK};
	}

	void handle_interrupt() {
		auto intr = read<Interrupt>();
		auto intrA = read<InterruptA>();
		auto intrB = read<InterruptB>();

		pr_debug("Int Flags = 0x%x VBusOK(rising edge intr.)=%d, BCLVL=%d\n", (uint8_t)intr, intr.VBusOK, intr.BCLevel);
		pr_debug("IntA = 0x%x IntB = 0x%x TogDone=%d\n", (int)intrA, (int)intrB, intrA.ToggleDone);

		switch (state) {
			case ConnectedState::TogglePolling: {
				pr_debug("State is currently Polling\n");

				Status0 status0{read<Status0>()};
				Status1A status1a{read<Status1A>()};

				if (status1a.ToggleOutcomeIsSink) {
					// The toggle settled as an attached sink: the partner presents Rp,
					// so it is the host (whether or not it sources VBUS).
					// Take over from the toggle state machine: stop it, present Rd on
					// both CCs and measure the settled CC, so BC_LVL detach detection
					// and the PD engine's BMC TX/RX have a defined, live CC.
					bool cc_is_cc2 = status1a.ToggleOutcomeIsCC2;
					write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});
					write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 1, .MeasuringBlock = 1, .IntOsc = 0});
					write<Switches0>(
						{.PullDownCC1 = 1, .PullDownCC2 = 1, .MeasureCC1 = !cc_is_cc2, .MeasureCC2 = cc_is_cc2});
					state = ConnectedState::AsDevice;
					pd.enable(cc_is_cc2);
				}

				else if (status0.VBusOK)
				{
					// The toggle outcome and VBUS disagree:
					// We got an interrupt that VBUS is changed (from the partner: we never drive it while toggling).
					// And we did not get an interrupt that the Toggle Outcome was SNK (therefore
					// we are the SRC/host or maybe we mis-read an open CC line)
					//
					// This could mean a problem unit (acts as a Device, supplies VBUS, toggles Rp)
					// or it could mean a normal Host and we just mis-setttled on the toggle read.
					//
					// Distinguish them by measuring CC ourselves: stop the toggle,
					// present Rd on both CC pins, and look for the partner's Rp.

					write<Control2>({.Toggle = 0, .PollingMode = 0, .ToggleIgnoreRa = 1});

					// Enable MeasuringBlock (the line above powered it down via stopping toggling).
					write<Power>({.BandGapAndWake = 1, .MeasureRefsAndRX = 1, .MeasuringBlock = 1, .IntOsc = 0});

					// Check both CC lines to see if either one has an Rp (e.g. we see a host connected).
					// Present an Rd, pause, then measure one CC line. Then repeat, measuring the other CC line.
					bool cc_is_cc2 = status1a.ToggleOutcomeIsCC2;
					bool host_rp_found = false;
					bool force_host = last_polling_mode == Control2::PollSRC;
					// If we are in Force-Host mode (PollSRC) then skip this check since
					// we are not interested in being a device and presenting an Rd
					// could tell the upstream Host we're a device.
					if (!force_host) {
						for (auto tries = 0; tries < 2; tries++) {
							write<Switches0>({.PullDownCC1 = 1,
											  .PullDownCC2 = 1,
											  .MeasureCC1 = !cc_is_cc2,
											  .MeasureCC2 = cc_is_cc2});
							HAL_Delay(2); // let the BC_LVL comparator settle
							Status0 probe{read<Status0>()};
							pr_debug("Present Rd, look for Rp: CC%d BCLevel=%d\n", cc_is_cc2 ? 2 : 1, probe.BCLevel);
							if (probe.BCLevel > 0) {
								host_rp_found = true;
								// We have VBUS and we detected an Rp -- so we found a potential host.
								// If it turns out to be a "problem unit" and the Rp was just part
								// of a toggling CC, then we'll fail to connect and the AsDevice
								// handler below will fire when the OXI toggles the CC line low.
								// Note: whichever CC we detected the BCLevel on is the active one,
								// so keep it selected.
								break;
							}
							cc_is_cc2 = !cc_is_cc2;
						}
					}

					// If we didn't see an Rp, but we still have VBUS then look for
					// toggling CC lines over a period of 120ms.
					// Measure the CC levels for 6 rounds x 2 CCs x 10ms = 120ms
					bool partner_toggling = false;
					if (!host_rp_found && !force_host) {
						for (auto rounds = 0; rounds < 6 && !host_rp_found; rounds++) {
							for (uint8_t probe_cc2 = 0; probe_cc2 <= 1 && !host_rp_found; probe_cc2++) {
								write<Switches0>({.PullDownCC1 = 1,
												  .PullDownCC2 = 1,
												  .MeasureCC1 = uint8_t(probe_cc2 ? 0 : 1),
												  .MeasureCC2 = probe_cc2});
								HAL_Delay(10);
								Status0 probe{read<Status0>()};
								if (probe.BCLevel > 0) {
									pr_debug("Toggle detection: CC%d BCLevel=%d\n", probe_cc2 ? 2 : 1, probe.BCLevel);
									host_rp_found = true; // leave this CC selected for detach detection
									cc_is_cc2 = probe_cc2;
									// The previous probe just saw this partner NOT
									// presenting Rp, therefore it is toggling Rp/Rd
									partner_toggling = true;
								}
							}
						}
					}

					if (partner_toggling) {
						pr_debug("Toggling self-powered partner: presenting Rp to host it\n");
						attach_as_static_src(cc_is_cc2);
					}

					else if (host_rp_found)
					{
						// A steady host Rp is out there: attach as a device/sink. The live
						// CC is left selected for measurement, so detach detection
						// (VBUS loss or BC_LVL 0) keeps working.
						state = ConnectedState::AsDevice;
						pd.enable(cc_is_cc2);
					} else {
						// No Rp on either CC but yet we have VBUS. It must be backfed by a self-powered
						// device, and the toggle's SRC outcome was right. Attach as a host.
						// First, figure out which CC line is active.
						cc_is_cc2 = status1a.ToggleOutcomeIsCC2;
						bool device_rd_found = false;
						for (auto tries = 0; tries < 2; tries++) {
							write<Switches0>(
								{.MeasureCC1 = !cc_is_cc2, .MeasureCC2 = cc_is_cc2, .PullUpCC1 = 1, .PullUpCC2 = 1});
							HAL_Delay(2); // let the BC_LVL comparator settle
							Status0 probe{read<Status0>()};
							pr_debug("Source override: CC%d BCLevel=%d\n", cc_is_cc2 ? 2 : 1, probe.BCLevel);
							// With Rp presented, BCLevel means:
							// 3 is an open line
							// 1 or 2 means a device's Rd.
							// 0 is Ra only (powered cable)
							if (probe.BCLevel == 1 || probe.BCLevel == 2) {
								device_rd_found = true;
								break;
							}
							cc_is_cc2 = !cc_is_cc2;
						}

						if (device_rd_found) {
							// Probe CC's until we read a stable Rd (BCLevel=1 or 2) for 150ms
							uint8_t stable = 1;
							for (auto t = 0; t < 100 && stable < 15; t++) {
								HAL_Delay(10);
								Status0 probe{read<Status0>()};
								stable = (probe.BCLevel == 1 || probe.BCLevel == 2) ? stable + 1 : 0;
							}
							if (stable >= 15) {
								pr_debug("Partner Rd committed on CC%d, attaching as host\n", cc_is_cc2 ? 2 : 1);
								// VBUS is partner-backfed, so detach on VBUS loss not on CC change
								host_partner_sources_vbus = true;
								state = ConnectedState::AsHost;
							} else {
								pr_debug("Partner Rd did not commit, re-polling\n");
								start_toggle_polling(last_polling_mode);
							}
						} else {
							// Neither Rp nor Rd anywhere despite VBUS: transient
							// (partner mid-plug or mid-power-up). Re-arm the toggle
							// and keep polling
							start_toggle_polling(last_polling_mode);
						}
					}
				}

				else if (status1a.ToggleOutcomeIsCC1 || status1a.ToggleOutcomeIsCC2)
				{
					// ToggleOutcomeIsSink did not fire, and VBUS change interrupt did not fire
					// so we must be the SRC (Host).
					state = ConnectedState::AsHost;
				}

				if (state == ConnectedState::AsDevice)
					// As a device (sink), the clean detach signal is VBUS loss, so
					// unmask the VBusOK interrupt. However, this is just our best effort
					// because in the field sometimes we do not see a VBusOK interrupt
					// fire when VBUS goes low. The manager's polling should catch it
					// in that case.
					// CRCCheck also unmasked: INT_N must assert on each received PD
					// packet so the PD engine can respond within tSenderResponse
					write<Mask>({.HostCurrentReq = 0,
								 .Collision = 1,
								 .Wake = 1,
								 .Alert = 1,
								 .CRCCheck = 0,
								 .CompChange = 1,
								 .CCBusActivity = 1,
								 .VBusOK = 0});
			} break;

			case ConnectedState::AsDevice: {
				pr_debug("State is currently Device\n");

				// PD protocol servicing (uses the just-read InterruptA flags)
				pd.service(intrA);

				Status0 status0{read<Status0>()};

				// Look for unplug event:
				// VBusOK = 0 means no VBUS, BCLevel == 0 means CC detected as low (no
				// host pull-up detected).
				//
				// Debounce detecting a low CC/VBUS as a disconnect event.
				// Only detach after the link has read down for LinkDownDebounceMs.
				// Persistent outages are re-read by the ~250ms link poll in UsbManager,
				// so a real unplug is confirmed in roughly LinkDownDebounceMs + one poll
				// period. A gap of more than LinkDownEpisodeMs between bad reads starts a
				// new episode (a good read in between isn't guaranteed to reach us: the backstop
				// only calls in when it sees a bad status).
				if (status0.VBusOK == 0 || status0.BCLevel == 0) {
					if (!link_down_debounce_enabled && status0.VBusOK && status0.BCLevel == 0) {
						// CC dropped while VBUS stays, before enumeration: not an unplug
						// (a real unplug drops both) -- the partner is a toggling self-powered unit
						// (OXI "Device Self Powered"). It will never enumerate us and never holds a
						// CC role, but it latches its device persona on seeing steady Rp. Flip to
						// the source persona and host it.
						auto sw = read<Switches0>();
						pr_debug("CC dropped, VBUS present: toggling self-powered partner; presenting Rp to host it\n");
						attach_as_static_src(sw.MeasureCC2);
						break;
					}
					if (!link_down_debounce_enabled) {
						// Not yet enumerated: treat the dip as a real detach right away
						pr_debug("Device link down (Status0=0x%x VBusOK=%d BCLevel=%d), detaching\n",
								 (uint8_t)status0,
								 status0.VBusOK,
								 status0.BCLevel);
						state = ConnectedState::None;
						break;
					}

					auto now = HAL_GetTick();
					if (link_down_since == 0 || now - link_down_last_seen > LinkDownEpisodeMs) {
						link_down_since = now ? now : 1;
						pr_debug("Device link down (Status0=0x%x VBusOK=%d BCLevel=%d), debouncing\n",
								 (uint8_t)status0,
								 status0.VBusOK,
								 status0.BCLevel);
					} else if (now - link_down_since >= LinkDownDebounceMs) {
						pr_debug("Device link down confirmed (Status0=0x%x VBusOK=%d BCLevel=%d)\n",
								 (uint8_t)status0,
								 status0.VBusOK,
								 status0.BCLevel);
						link_down_since = 0;
						state = ConnectedState::None;
					}
					link_down_last_seen = now;
				} else {
					if (link_down_since)
						pr_debug("Device link restored (Status0=0x%x), ignoring dip\n", (uint8_t)status0);
					link_down_since = 0;
				}
			} break;

			case ConnectedState::AsHost: {
				pr_debug("State is currently Host\n");
				Status0 status{read<Status0>()};

				if (host_partner_sources_vbus) {
					// Partner sources VBUS (toggling self-powered DRP / gadget rig):
					// VBUS-loss is the only trustworthy detach signal
					if (status.VBusOK == 0) {
						pr_debug("Partner VBUS gone, detaching\n");
						state = ConnectedState::None;
					}
					break;
				}

				// Look for Unplug event:
				// Comp==1, BC==3 means CC pin is read as > 1.23V, meaning no device Rd
				// pull-down
				// FIXME: why isn't Comp set at this point? it gets set a moment later...
				if (/*status.Comp == 1 &&*/ status.BCLevel == 3) {
					// Confirm before detaching: the open reading must out-last a
					// DRP toggle's Rp phase (~30-70ms) -- a partner still mid-
					// commit (OXI One) swings its Rd away and back, and single CC
					// reads here have a history of transients (see FIXME above).
					// A real unplug reads open throughout; the 150ms of extra
					// detach latency is imperceptible.
					bool detached = true;
					for (auto i = 0; i < 15; i++) {
						HAL_Delay(10);
						Status0 confirm{read<Status0>()};
						if (confirm.BCLevel != 3) {
							pr_debug("Host CC open was transient (BCLevel=%d), staying attached\n", confirm.BCLevel);
							detached = false;
							break;
						}
					}
					if (detached)
						state = ConnectedState::None;
				}
			} break;

			case ConnectedState::None: {
				pr_debug("State is currently None\n");
			} break;
		}
	}

	// Returns true on success
	template<typename Reg>
	bool write(Reg data) {
		return i2c.write_reg(dev_addr, data) == mdrivlib::I2CPeriph::I2C_NO_ERR;
	}

	template<typename Reg>
	Reg read() {
		static bool got_error_once = false;
		auto reg = i2c.read_reg<Reg>(dev_addr);

		if (!reg.has_value() && !got_error_once) {
			pr_debug("Error reading Reg 0x%x\n", Reg::Address);
			got_error_once = true;
		}
		return reg.value_or(Reg::make(0));
	}

	//// Debugging stuff:
#ifdef FUSBDEBUG
	template<typename Reg>
	requires std::derived_from<Reg, BusReg::ReadAccess>
	void reg_check_changed(std::string_view regname) {
		static uint8_t last_val = 0xFF;
		auto val = read<Reg>();
		if ((uint8_t)val != last_val)
			pr_debug("Changed %s: 0x%x\n", regname.data(), (int)val);
		last_val = val;
	}

	template<typename Reg>
	requires std::derived_from<Reg, BusReg::ReadAccess>
	void reg_dump(std::string_view regname) {
		auto val = read<Reg>();
		pr_debug("%s: 0x%x\n", regname.data(), (uint8_t)val);

		// Dump individual fields of interest:
		if constexpr (static_cast<uint8_t>(Reg::Address) == FUSB302::Status0::Address) {
			auto s = Status0::make(val);
			pr_debug("   BCLevel=%d Wake=%d Comp=%d VBusOK=%d\n", s.BCLevel, s.Wake, s.Comp, s.VBusOK);
		}
	}

	void check_all_regs() {
		reg_check_changed<ID>("ID");
		reg_check_changed<Switches0>("Switches0");
		reg_check_changed<Switches1>("Switches1");
		reg_check_changed<Measure>("Measure");
		reg_check_changed<Slice>("Slice");
		reg_check_changed<Control0>("Control0");
		reg_check_changed<Control1>("Control1");
		reg_check_changed<Control2>("Control2");
		reg_check_changed<Control3>("Control3");
		reg_check_changed<Mask>("Mask1");
		reg_check_changed<Power>("Power");
		reg_check_changed<OCP>("OCP");
		reg_check_changed<MaskA>("MaskA");
		reg_check_changed<MaskB>("MaskB");
		reg_check_changed<Control4>("Control4");
		reg_check_changed<Status0A>("Status0A");
		reg_check_changed<Status1A>("Status1A");
		reg_check_changed<Status0>("Status0");
		reg_check_changed<Status1>("Status1");
	}

	void dump_all_regs() {
		reg_dump<ID>("ID");
		reg_dump<Switches0>("Switches0");
		reg_dump<Switches1>("Switches1");
		reg_dump<Measure>("Measure");
		reg_dump<Slice>("Slice");
		reg_dump<Control0>("Control0");
		reg_dump<Control1>("Control1");
		reg_dump<Control2>("Control2");
		reg_dump<Control3>("Control3");
		reg_dump<Mask>("Mask1");
		reg_dump<Power>("Power");
		reg_dump<OCP>("OCP");
		reg_dump<MaskA>("MaskA");
		reg_dump<MaskB>("MaskB");
		reg_dump<Control4>("Control4");
		reg_dump<Status0A>("Status0A");
		reg_dump<Status1A>("Status1A");
		reg_dump<Status0>("Status0");
		reg_dump<Status1>("Status1");
	}
#else
	void dump_all_regs() {
	}
	void check_all_regs() {
	}
#endif
};

} // namespace FUSB302
