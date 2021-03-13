#include "asm_macro.h"

.macro set_pt_regs
    la t0, regs
    sd x10,0*8(t0)
    sd x11,1*8(t0)
    sd x12,2*8(t0)
    sd x13,3*8(t0)
    sd x14,4*8(t0)
    sd x15,5*8(t0)
    sd x16,6*8(t0)
    sd x17,7*8(t0)
.endm

.macro get_pt_regs
    la x17, regs
    ld x10,0*8(x17)
    ld x11,1*8(x17)
    ld x12,2*8(x17)
    ld x13,3*8(x17)
    ld x14,4*8(x17)
    ld x15,5*8(x17)
    ld x16,6*8(x17)
    ld x17,7*8(x17)
.endm

.globl trap_s

.section .text.entry

trap_s:
csrrw sp, sscratch, sp # use S.sp, save U.sp
pusha
set_pt_regs #! before any C function
jal swap_satp # use Kernel PT
csrr a0, scause
bltz a0, sint_srv
secp_srv:
    # prep handler_s arguments
    la a2, regs
    csrr a1, sepc
    jal handler_s
    # sepc += 4
    csrr a2, sepc
    addi a2, a2, 4
    csrw sepc, a2
    secp_ret:
    jal swap_satp # use User PT
    # pop GPR from kernel stack
    popa
    get_pt_regs #! after any C function
    csrrw sp, sscratch, sp # use U.sp, save S.sp
    sret
sint_srv:
sti_srv:
    jal do_timer
    # may be another context after sched
    # push sepc for subsequent ecall
    csrr a1,sepc
    addi sp,sp,-8
    sd a1,0(sp)
    # inform M-mode
    ecall
    # pop sepc
    ld a1,0(sp)
    addi sp,sp,8
    csrw sepc,a1
sint_ret:
jal swap_satp
# pop GPR from kern stack of on-go task
popa
csrrw sp, sscratch, sp # use U.sp, save S.sp
sret