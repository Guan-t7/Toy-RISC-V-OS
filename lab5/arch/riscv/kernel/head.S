#include "asm_macro.h"

.globl _start

.section .text.init

.equ MTIME, 0x0200bff8
.equ MTIMECMP, 0x02004000

# M mode entry
_start:
# MMU: Bare
csrw satp, zero
# Reset CSRs[mstatus].MIE
csrci mstatus, 0b01000
# M mode 异常处理地址
la s0, trap_m
csrw mtvec, s0
# Supervisor mode start address
la s1, S_start
csrw mepc, s1
# M mode stack
la sp, stack_top 
csrw mscratch, sp
# Zero bss
la a0, bss_start
la a1, bss_end
sub a2, a1, a0
mv a1, zero
jal memset
# initialize mtimecmp
li a1, MTIME
li a2, MTIMECMP
ld t0, 0(a1)
li t1, 1000000
add t2, t0, t1 # (mtime)+1000000
sd t2, 0(a2)
# Machine Timer Interrupt bit
li a0, 0b010000000
# Supervisor Timer Interrupt bit
li a1, 0b000100000
csrs mideleg, a1
csrs mie, a0
csrs mie, a1
# delegate INSTR_PF, LOAD_PF, STORE_PF, ECALL_U to S mode
li s3, 0xB100
csrs medeleg, s3
# Set CSRs[mstatus].MIE
csrsi mstatus, 0b01000
# CSRs[mstatus].MPP = 0b01, CSRs[mstatus].MPIE = 1
li a1, 0b01100000000000
li a2, 0b00100010000000
csrc mstatus, a1
csrs mstatus, a2
# to S mode
mret

S_start:
# enable SUM
li t0, 0x40000
csrs sstatus, t0
# init page table
jal paging_init
# Mode: Sv39
li s6, 8
slli s6, s6, 60
# Root pgtbl PPN
srli s7, a0, 12
# MMU: Sv39 Paging
or s8, s6, s7
csrw satp, s8
sfence.vma
# PC is still in PA. In order to switch to VA,
# we calc some known pos in VA and jr there
la s9, next
#TODO do not hard code
li a7, 0xffffffe000000000 - 0x0000000080000000
add s9, s9, a7
# jump to next
jr s9
next:
# PC is in VA now.
# S mode 异常处理地址
la s10, trap_s
csrw stvec, s10
# S mode stack
la sp, init_stack_top
csrw sscratch, sp
jal start_kernel

.equ ECALL_S, 9

trap_m:
    csrrw sp, mscratch, sp # swap sp with M.sp
    pusha
    li s0, 0b010000000
    li s1, 0b000100000
    li s2, MTIMECMP
    # check cause
    csrr s11, mcause
    bltz s11, int_srv
    ecp_srv:
        li s6, ECALL_S
        beq s6, s11, ecall_srv
            csrr a0, mcause
            csrr a1, mepc
            jal handler_m
        ecall_srv:
            # set mtimecmp += 1000000
            ld t0, 0(s2)
            li t1, 1000000
            add t2, t0, t1
            sd t2, 0(s2)
            # enable mie[mtie]
            csrs mie, s0
            # disable mip[stip]
            csrc mip, s1
            # mepc += 4
            csrr s10, mepc
            addi s10, s10, 4
            csrw mepc, s10
            beq zero, zero, trap_m_ret
    int_srv:
    mti_srv:
        # disable mie[mtie]
        csrc mie, s0
        # enable mip[stip]
        csrs mip, s1
    trap_m_ret:
    popa
    csrrw sp, mscratch, sp # swap M.sp with sp
    mret