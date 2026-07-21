// Weak hooks (override by defining the symbol in your project):
//   early_print_el        - e.g. print the current EL to a UART (before anything else)
//   block_ram_enable_el3  - e.g. open SYSRAM/SRAM/RETRAM via RISAB (see drivers/risab.hh)
//   system_init           - clock/PLL setup, called just before main()
//
// Required from the app: mmu_enable / mmu_enable_el3 (copy templates/mmu_ca35.cc),
// vectors (boot/vectors.S), IRQ_Dist_Initialize / IRQ_Initialize (boot/irq_init.c),
// and the linker symbols _stack_start/_bss_start/_bss_end (templates linker script).

.cpu cortex-a35

    .weak early_print_el
early_print_el:
    ret

    .weak block_ram_enable_el3
block_ram_enable_el3:
    ret

    .weak system_init
system_init:
    ret

    .section .text.boot, "ax"
    .global _start
    .global _Reset
_Reset:
    // Mask interrupts
    msr     daifset, #0xf

    // Set up stack
    ldr     x1, =_stack_start
    bic     sp, x1, 0xf

    // Clear .bss
    ldr     x0, =_bss_start
    ldr     x1, =_bss_end
    mov     x2, #0
bss_loop:
    cmp     x0, x1
    b.hs    bss_done
    str     x2, [x0], #8
    b       bss_loop
bss_done:

    bl      early_print_el
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    and     x0, x0, #0x3
    cmp     x0, #3
    b.eq    el3_entry
    cmp     x0, #2
    b.eq    el2_entry
    cmp     x0, #1
    b.eq    el1_entry

// EL0 entry: hang -- not supported
el0_entry:
    b       el0_entry

// EL2 entry: drop down to EL1
el2_entry:
    mov     x2, #(0x5)              // EL1h
    orr     x2, x2, #(0xF << 6)     // mask D,A,I,F
    msr     spsr_el2, x2
    // Set ELR_EL2 to EL1 entry point and provide SP_EL1
    adr     x3, el1_entry
    msr     elr_el2, x3
    msr     sp_el1, x1
    // Ensure EL1 is AArch64 (RW=1 in HCR_EL2)
    mrs     x1, hcr_el2
    orr     x1, x1, #(1 << 31)      // HCR_EL2.RW = 1 => EL1 is AArch64
    msr     hcr_el2, x1
    isb
    eret

// EL1 entry:
el1_entry:
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
    b       entry_done

// EL3 entry:
el3_entry:
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

    bl      block_ram_enable_el3

    mrs     x1, scr_el3
    orr     x1, x1, #(1 << 1)       // IRQ at EL3
    orr     x1, x1, #(2 << 1)       // FIQ at EL3
    msr     scr_el3, x1

    b       entry_done

entry_done:
    bl      __libc_init_array
    msr     daifclr, #0xf
    bl      IRQ_Dist_Initialize
    bl      IRQ_Initialize

    bl      system_init
    bl      main

hang:
    wfe
    b       hang
