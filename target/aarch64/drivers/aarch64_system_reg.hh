#pragma once
#include <cstdint>

inline uint64_t get_core_id() {
	uint64_t core_id;
	asm volatile("mrs %0, mpidr_el1\n\t" : "=r"(core_id) : : "memory");
	uint64_t aff3 = (core_id >> 32) & 0xFF;
	return aff3;
}

inline uint64_t get_mpid() {
	uint64_t core_id;
	asm volatile("mrs %0, mpidr_el1\n\t" : "=r"(core_id) : : "memory");
	// asm volatile("MRS      %0, MPIDR_EL1   \n"
	// 			 "UBFX     %1, %0, #32, #8 \n" // Extract Aff3
	// 			 "BFI      %0, %1, #24, #8 \n" // Insert Aff3 into bits [31:24], so that [31:0]
	// 										   // is now Aff3.Aff2.Aff1.Aff0
	// );
	uint64_t aff3 = (core_id >> 8) & 0xFF00'0000;
	return (core_id & 0x00FF'FFFF) | aff3;
}

inline uint32_t read_current_el() {
	uint64_t current_el;
	asm volatile("mrs %0, CurrentEL\n\t" : "=r"(current_el) : : "memory");
	return current_el;
}

inline uint32_t get_current_el(void) {
	constexpr uint64_t CURRENT_EL_MASK = 0x3;
	constexpr uint64_t CURRENT_EL_SHIFT = 2;

	uint32_t current_el = read_current_el();
	return ((current_el >> CURRENT_EL_SHIFT) & CURRENT_EL_MASK);
}

// HCR
inline uint64_t read_hcr_el2(void) {
	uint64_t reg;
	asm volatile("mrs %0, HCR_EL2\n\t" : "=r"(reg) : : "memory");
	return reg;
}

// SPSR
inline uint64_t read_spsr_el1() {
	uint64_t spsr_el1;
	asm volatile("mrs %0, SPSR_EL1\n\t" : "=r"(spsr_el1) : : "memory");
	return spsr_el1;
}

inline void write_spsr_el1(uint64_t spsr_el1) {
	asm volatile("msr SPSR_EL1, %0\n\t" : : "r"(spsr_el1) : "memory");
}

inline uint64_t read_spsr_el2() {
	uint64_t spsr_el2;
	asm volatile("mrs %0, SPSR_EL2\n\t" : "=r"(spsr_el2) : : "memory");
	return spsr_el2;
}

inline void write_spsr_el2(uint64_t spsr_el2) {
	asm volatile("msr SPSR_EL2, %0\n\t" : : "r"(spsr_el2) : "memory");
}

// VBAR

inline uint64_t read_vbar_el2() {
	uint64_t vbar_el2;

	asm volatile("mrs %0, VBAR_EL2\n\t" : "=r"(vbar_el2) : : "memory");
	return vbar_el2;
}

inline uint64_t read_vbar_el1() {
	uint64_t vbar_el1;

	asm volatile("mrs %0, VBAR_EL1\n\t" : "=r"(vbar_el1) : : "memory");
	return vbar_el1;
}

inline void write_vbar_el1(uint64_t vbar_el1) {
	asm volatile("msr VBAR_EL1, %0\n\t" : : "r"(vbar_el1) : "memory");
}

// DAIF

constexpr uint64_t DAIF_DBG_BIT = (1 << 3); // Debug mask bit
constexpr uint64_t DAIF_ABT_BIT = (1 << 2); // Asynchronous abort mask bit
constexpr uint64_t DAIF_IRQ_BIT = (1 << 1); // IRQ mask bit
constexpr uint64_t DAIF_FIQ_BIT = (1 << 0); // FIQ mask bit

inline uint32_t read_daif() {
	uint32_t daif;

	asm volatile("mrs %0, DAIF\n\t" : "=r"(daif) : : "memory");
	return daif;
}

inline void write_daif(uint32_t daif) {
	asm volatile("msr DAIF, %0\n\t" : : "r"(daif) : "memory");
}

inline void enable_debug_exceptions() {
	asm volatile("msr DAIFClr, %0\n\t" : : "i"(DAIF_DBG_BIT) : "memory");
}

inline void enable_serror_exceptions() {
	asm volatile("msr DAIFClr, %0\n\t" : : "i"(DAIF_ABT_BIT) : "memory");
}

inline void enable_irq() {
	asm volatile("msr DAIFClr, %0\n\t" : : "i"(DAIF_IRQ_BIT) : "memory");
}

inline void enable_fiq() {
	asm volatile("msr DAIFClr, %0\n\t" : : "i"(DAIF_FIQ_BIT) : "memory");
}

inline void disable_debug_exceptions() {
	asm volatile("msr DAIFSet, %0\n\t" : : "i"(DAIF_DBG_BIT) : "memory");
}

inline void disable_serror_exceptions() {
	asm volatile("msr DAIFSet, %0\n\t" : : "i"(DAIF_ABT_BIT) : "memory");
}

inline void disable_irq() {
	asm volatile("msr DAIFSet, %0\n\t" : : "i"(DAIF_IRQ_BIT) : "memory");
}

inline void disable_fiq() {
	asm volatile("msr DAIFSet, %0\n\t" : : "i"(DAIF_FIQ_BIT) : "memory");
}
