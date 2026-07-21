#ifdef RUN_EL3

.macro get_esr_elx reg
    mrs     \reg, esr_el3
.endm

.macro get_elr_elx reg
    mrs     \reg, elr_el3
.endm

.macro get_spsr_elx reg
    mrs     \reg, spsr_el3
.endm

.macro set_esr_elx reg
    msr     esr_el3, \reg
.endm

.macro set_elr_elx reg
    msr     elr_el3, \reg
.endm

.macro set_spsr_elx reg
    msr     spsr_el3, \reg
.endm

#else

.macro get_esr_elx reg
    mrs     \reg, esr_el1
.endm

.macro get_elr_elx reg
    mrs     \reg, elr_el1
.endm

.macro get_spsr_elx reg
    mrs     \reg, spsr_el1
.endm

.macro set_esr_elx reg
    msr     esr_el1, \reg
.endm

.macro set_elr_elx reg
    msr     elr_el1, \reg
.endm

.macro set_spsr_elx reg
    msr     spsr_el1, \reg
.endm

#endif
