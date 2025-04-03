#pragma once
#include <cstdint>

inline uint64_t get_core_id() {
	return 0;
}

inline uint64_t get_mpid() {
	return 0;
}

inline uint32_t read_current_el() {
	return 2;
}

inline uint32_t get_current_el(void) {
	return 2;
}

// HCR
inline uint64_t read_hcr_el2(void) {
	uint64_t reg;
	return reg;
}

// SPSR
inline uint64_t read_spsr_el1() {
	uint64_t spsr_el1;
	return spsr_el1;
}

inline void write_spsr_el1(uint64_t spsr_el1) {
}

inline uint64_t read_spsr_el2() {
	uint64_t spsr_el2;
	return spsr_el2;
}

inline void write_spsr_el2(uint64_t spsr_el2) {
}

// VBAR

inline uint64_t read_vbar_el2() {
	uint64_t vbar_el2;
	return vbar_el2;
}

inline uint64_t read_vbar_el1() {
	uint64_t vbar_el1;
	return vbar_el1;
}

inline void write_vbar_el1(uint64_t vbar_el1) {
}

// DAIF

constexpr uint64_t DAIF_DBG_BIT = (1 << 3); // Debug mask bit
constexpr uint64_t DAIF_ABT_BIT = (1 << 2); // Asynchronous abort mask bit
constexpr uint64_t DAIF_IRQ_BIT = (1 << 1); // IRQ mask bit
constexpr uint64_t DAIF_FIQ_BIT = (1 << 0); // FIQ mask bit

inline uint32_t read_daif() {
	uint32_t daif;
	return daif;
}

inline void write_daif(uint32_t daif) {
}

inline void enable_debug_exceptions() {
}

inline void enable_serror_exceptions() {
}

inline void enable_irq() {
}

inline void enable_fiq() {
}

inline void disable_debug_exceptions() {
}

inline void disable_serror_exceptions() {
}

inline void disable_irq() {
}

inline void disable_fiq() {
}
