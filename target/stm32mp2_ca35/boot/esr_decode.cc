// esr_decode.cc - Decode and print AArch64 Exception Syndrome Register (ESR)
#include <stdint.h>

extern "C" void putchar_s(const char c);
extern "C" void early_puts(const char *s);

static void put_hex_byte(unsigned n)
{
	const char hex[] = "0123456789abcdef";
	putchar_s(hex[(n >> 4) & 0xF]);
	putchar_s(hex[n & 0xF]);
}

static void print_fault_status(unsigned fsc)
{
	switch (fsc & 0x3F) {
	case 0x00: early_puts("Address size fault, level 0"); break;
	case 0x01: early_puts("Address size fault, level 1"); break;
	case 0x02: early_puts("Address size fault, level 2"); break;
	case 0x03: early_puts("Address size fault, level 3"); break;
	case 0x04: early_puts("Translation fault, level 0"); break;
	case 0x05: early_puts("Translation fault, level 1"); break;
	case 0x06: early_puts("Translation fault, level 2"); break;
	case 0x07: early_puts("Translation fault, level 3"); break;
	case 0x08: early_puts("Access flag fault, level 0"); break;
	case 0x09: early_puts("Access flag fault, level 1"); break;
	case 0x0A: early_puts("Access flag fault, level 2"); break;
	case 0x0B: early_puts("Access flag fault, level 3"); break;
	case 0x0C: early_puts("Permission fault, level 0"); break;
	case 0x0D: early_puts("Permission fault, level 1"); break;
	case 0x0E: early_puts("Permission fault, level 2"); break;
	case 0x0F: early_puts("Permission fault, level 3"); break;
	case 0x10: early_puts("Sync external abort (not on TTW)"); break;
	case 0x11: early_puts("Sync tag check fail"); break;
	case 0x14: early_puts("Sync external abort on TTW, level 0"); break;
	case 0x15: early_puts("Sync external abort on TTW, level 1"); break;
	case 0x16: early_puts("Sync external abort on TTW, level 2"); break;
	case 0x17: early_puts("Sync external abort on TTW, level 3"); break;
	case 0x18: early_puts("Sync parity/ECC error (not on TTW)"); break;
	case 0x1C: early_puts("Sync parity/ECC error on TTW, level 0"); break;
	case 0x1D: early_puts("Sync parity/ECC error on TTW, level 1"); break;
	case 0x1E: early_puts("Sync parity/ECC error on TTW, level 2"); break;
	case 0x1F: early_puts("Sync parity/ECC error on TTW, level 3"); break;
	case 0x21: early_puts("Alignment fault"); break;
	case 0x22: early_puts("Debug exception"); break;
	default:   early_puts("Unknown fault status"); break;
	}
}

extern "C" void print_esr_description(uint64_t esr)
{
	unsigned ec  = (esr >> 26) & 0x3F;
	unsigned il  = (esr >> 25) & 0x01;
	unsigned iss = (unsigned)(esr & 0x1FFFFFF);

	early_puts("  EC=0x");
	put_hex_byte(ec);
	early_puts(": ");

	switch (ec) {
	case 0x00: early_puts("Unknown/uncategorized"); break;
	case 0x01: early_puts("Trapped WFI/WFE"); break;
	case 0x03: early_puts("Trapped MCR/MRC (CP15)"); break;
	case 0x04: early_puts("Trapped MCRR/MRRC (CP15)"); break;
	case 0x05: early_puts("Trapped MCR/MRC (CP14)"); break;
	case 0x06: early_puts("Trapped LDC/STC (CP14)"); break;
	case 0x07: early_puts("Trapped SVE/SIMD/FP access"); break;
	case 0x0C: early_puts("Trapped MRRC (CP14)"); break;
	case 0x0D: early_puts("Branch Target Exception"); break;
	case 0x0E: early_puts("Illegal execution state"); break;
	case 0x11: early_puts("SVC (AArch32)"); break;
	case 0x12: early_puts("HVC (AArch32)"); break;
	case 0x13: early_puts("SMC (AArch32)"); break;
	case 0x15: early_puts("SVC (AArch64)"); break;
	case 0x16: early_puts("HVC (AArch64)"); break;
	case 0x17: early_puts("SMC (AArch64)"); break;
	case 0x18: early_puts("Trapped MSR/MRS/System instruction"); break;
	case 0x19: early_puts("Trapped SVE access"); break;
	case 0x1A: early_puts("Trapped ERET/ERETAA/ERETAB"); break;
	case 0x1F: early_puts("Impl-defined exception to EL3"); break;
	case 0x20: early_puts("Instruction Abort (lower EL)"); break;
	case 0x21: early_puts("Instruction Abort (same EL)"); break;
	case 0x22: early_puts("PC alignment fault"); break;
	case 0x24: early_puts("Data Abort (lower EL)"); break;
	case 0x25: early_puts("Data Abort (same EL)"); break;
	case 0x26: early_puts("SP alignment fault"); break;
	case 0x27: early_puts("FP exception (AArch32)"); break;
	case 0x2C: early_puts("FP exception (AArch64)"); break;
	case 0x2F: early_puts("SError interrupt"); break;
	case 0x30: early_puts("Breakpoint (lower EL)"); break;
	case 0x31: early_puts("Breakpoint (same EL)"); break;
	case 0x32: early_puts("Software Step (lower EL)"); break;
	case 0x33: early_puts("Software Step (same EL)"); break;
	case 0x34: early_puts("Watchpoint (lower EL)"); break;
	case 0x35: early_puts("Watchpoint (same EL)"); break;
	case 0x38: early_puts("BKPT instruction (AArch32)"); break;
	case 0x3A: early_puts("Vector catch (AArch32)"); break;
	case 0x3C: early_puts("BRK instruction (AArch64)"); break;
	default:   early_puts("Reserved/unknown EC"); break;
	}
	early_puts("\n");

	early_puts("  IL: ");
	early_puts(il ? "32-bit" : "16-bit");
	early_puts(" instruction\n");

	// ISS decoding per EC
	if (ec == 0x20 || ec == 0x21) {
		// Instruction Abort
		early_puts("  IFSC: ");
		print_fault_status(iss & 0x3F);
		early_puts("\n");
		if ((iss >> 10) & 1)
			early_puts("  FAR not valid\n");

	} else if (ec == 0x24 || ec == 0x25) {
		// Data Abort
		early_puts("  DFSC: ");
		print_fault_status(iss & 0x3F);
		early_puts("\n");
		early_puts("  Access: ");
		early_puts((iss >> 6) & 1 ? "Write\n" : "Read\n");
		if ((iss >> 10) & 1)
			early_puts("  FAR not valid\n");
		if ((iss >> 7) & 1)
			early_puts("  Stage-1 TTW fault\n");

	} else if (ec == 0x11 || ec == 0x15) {
		// SVC
		early_puts("  SVC imm: 0x");
		put_hex_byte((iss >> 8) & 0xFF);
		put_hex_byte(iss & 0xFF);
		early_puts("\n");

	} else if (ec == 0x12 || ec == 0x16) {
		// HVC
		early_puts("  HVC imm: 0x");
		put_hex_byte((iss >> 8) & 0xFF);
		put_hex_byte(iss & 0xFF);
		early_puts("\n");

	} else if (ec == 0x13 || ec == 0x17) {
		// SMC
		early_puts("  SMC imm: 0x");
		put_hex_byte((iss >> 8) & 0xFF);
		put_hex_byte(iss & 0xFF);
		early_puts("\n");

	} else if (ec == 0x3C) {
		// BRK
		early_puts("  BRK comment: 0x");
		put_hex_byte((iss >> 8) & 0xFF);
		put_hex_byte(iss & 0xFF);
		early_puts("\n");
	}
}
