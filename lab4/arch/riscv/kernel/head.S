.globl _start

.section .text.init

.macro pusha
    addi sp,sp,-30*8
    sd x1,29*8(sp)
    sd x3,28*8(sp)
    sd x4,27*8(sp)
    sd x5,26*8(sp)
    sd x6,25*8(sp)
    sd x7,24*8(sp)
    sd x8,23*8(sp)
    sd x9,22*8(sp)
    sd x10,21*8(sp)
    sd x11,20*8(sp)
    sd x12,19*8(sp)
    sd x13,18*8(sp)
    sd x14,17*8(sp)
    sd x15,16*8(sp)
    sd x16,15*8(sp)
    sd x17,14*8(sp)
    sd x18,13*8(sp)
    sd x19,12*8(sp)
    sd x20,11*8(sp)
    sd x21,10*8(sp)
    sd x22,9*8(sp)
    sd x23,8*8(sp)
    sd x24,7*8(sp)
    sd x25,6*8(sp)
    sd x26,5*8(sp)
    sd x27,4*8(sp)
    sd x28,3*8(sp)
    sd x29,2*8(sp)
    sd x30,1*8(sp)
    sd x31,0*8(sp)
.endm

.macro popa
    ld x1,29*8(sp)
    ld x3,28*8(sp)
    ld x4,27*8(sp)
    ld x5,26*8(sp)
    ld x6,25*8(sp)
    ld x7,24*8(sp)
    ld x8,23*8(sp)
    ld x9,22*8(sp)
    ld x10,21*8(sp)
    ld x11,20*8(sp)
    ld x12,19*8(sp)
    ld x13,18*8(sp)
    ld x14,17*8(sp)
    ld x15,16*8(sp)
    ld x16,15*8(sp)
    ld x17,14*8(sp)
    ld x18,13*8(sp)
    ld x19,12*8(sp)
    ld x20,11*8(sp)
    ld x21,10*8(sp)
    ld x22,9*8(sp)
    ld x23,8*8(sp)
    ld x24,7*8(sp)
    ld x25,6*8(sp)
    ld x26,5*8(sp)
    ld x27,4*8(sp)
    ld x28,3*8(sp)
    ld x29,2*8(sp)
    ld x30,1*8(sp)
    ld x31,0*8(sp)
    addi sp,sp,30*8
.endm

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
# delegate INSTR_PF, LOAD_PF, STORE_PF to S mode
li s3, 0xD000
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
jal paging_init # init page table
# Mode: Sv39
li s6, 8
slli s6, s6, 60
# Root pgtbl located at _end
la s7, _end
srli s7, s7, 12
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
la sp, init_stack_top
# Set CSRs[sstatus].SIE
csrsi sstatus, 0b00010
jal start_kernel

.equ ECALL_S, 9

trap_m:
    csrrw sp, mscratch, sp # swap sp with mscratch
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
            la a0, ecp_fault
            jal puts
            j dead_loop
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
    csrrw sp, mscratch, sp # swap sp with mscratch
    mret

.section .rodata

ecp_fault:
    .asciz "FAULT\n"