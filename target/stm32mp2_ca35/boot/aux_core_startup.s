// Requires linker symbol _cpu1_stack_start and an aux_main() in the app.

.global aux_main
.global aux_core_startup
aux_core_startup:
    // Mask interrupts
    msr     daifset, #0xf

    // Set up stack
    ldr     x1, =_cpu1_stack_start
    mov     sp, x1

    // Branch to entry for our level
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    and     x0, x0, #0x3
    cmp     x0, #3
    b.eq    el3_entry_aux
    cmp     x0, #2
    b.eq    el2_entry_aux
    cmp     x0, #1
    b.eq    el1_entry_aux
    b       .               // EL0 hangs -- not supported


// EL2 entry: drop down to EL1
el2_entry_aux:
    mov     x2, #(0x5)              // EL1h
    orr     x2, x2, #(0xF << 6)     // mask D,A,I,F
    msr     spsr_el2, x2
    // Set ELR_EL2 to EL1 entry point and provide SP_EL1
    adr     x3, el1_entry_aux
    msr     elr_el2, x3
    msr     sp_el1, x1
    // Ensure EL1 is AArch64 (RW=1 in HCR_EL2)
    mrs     x1, hcr_el2
    orr     x1, x1, #(1 << 31)      // HCR_EL2.RW = 1 => EL1 is AArch64
    msr     hcr_el2, x1
    isb
    eret

// EL1 entry:
el1_entry_aux:
    // Install vector table for EL1
    ldr     x0, =vectors
    msr     vbar_el1, x0
    isb

    // Enable FP+SIMD at EL1
    mrs     x1, cpacr_el1
    orr     x1, x1, #3 << 20        // FPEN bits: don't trap FPU at EL1 or EL0
    msr     cpacr_el1, x1
    isb

    bl      mmu_enable              // enables MMU, Dcache, Icache in SCTLR
    b       entry_done_aux

// EL3 entry:
el3_entry_aux:
    ldr     x0, =vectors
    msr     vbar_el3, x0
    isb
    mrs     x0, cptr_el3
    bic     x0, x0, #(1 << 10)      // TFP = 0 do not trap fp/simd
    msr     cptr_el3, x0
    isb
    msr     fpcr, xzr
    msr     fpsr, xzr

    bl      mmu_enable_el3          // enables MMU, Dcache, Icache in SCTLR

    mrs     x1, scr_el3
    orr     x1, x1, #(1 << 1)       // IRQ at EL3
    orr     x1, x1, #(2 << 1)       // FIQ at EL3
    msr     scr_el3, x1

    b       entry_done_aux

entry_done_aux:
    bl      __libc_init_array
    msr     daifclr, #0xf
    bl      IRQ_Initialize
    bl      aux_main

aux_hang:
    wfe
    b       aux_hang
