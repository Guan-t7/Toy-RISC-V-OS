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

_start:
# We start in M mode
csrci mstatus, 0b01000 # Reset MIE in CSRs[mstatus] to disable interrupt
la t0, trap_m
csrw mtvec, t0 # 设置machine mode异常处理地址
la a0, S_start
csrw mepc, a0 # Supervisor mode start address

la a1, bss_start
la a2, bss_end
# It's stack after bss, tolerant against out-of-bounds
init_bss: 
sd zero, 0(a1)
addi a1, a1, 8
ble a1, a2, init_bss # if a1 <= a2 then loop
# initialize mtimecmp
li a1, MTIME
li a2, MTIMECMP
ld t0, 0(a1)
li t1, 1000000
add t2, t0, t1 # (mtime)+1000000
sd t2, 0(a2)
# Machine Timer Interrupt is at bit 7
li a0, 0b010000000
# Supervisor Timer Interrupt is at bit 5
li a1, 0b000100000
csrs mideleg, a1
csrs mie, a0
csrs mie, a1
# Set CSRs[mstatus].MIE, CSRs[mstatus].SIE
csrsi mstatus, 0b01010 
#! MPIE cannot affect SIE

# CSRs[mstatus].MPP = 0b01, CSRs[mstatus].MPIE = 1
li a1, 0b01100000000000
li a2, 0b00100010000000
csrc mstatus, a1
csrs mstatus, a2
# to supervisor mode
mret

S_start:
la t0, trap_s
csrw stvec, t0 # 设置supervisor mode异常处理地址
la sp, stack_top # 栈顶指针
j start_kernel

# M-mode下的时钟中断处理
trap_m0:
    pusha
    li s0, 0b010000000
    li s1, 0b000100000
    li s2, MTIMECMP
    # assertion: interrupt globally disabled by hardware on entry
    # print
    jal print_timer_int
    # set mtimecmp += 100000
    ld t0, 0(s2)
    li t1, 100000
    add t2, t0, t1
    sd t2, 0(s2)
    popa
    mret

trap_m:
    pusha
    li s0, 0b010000000
    li s1, 0b000100000
    li s2, MTIMECMP
    # check cause
    csrr s11, mcause
    bltz s11, mti_srv
    ecall_srv:
        # set mtimecmp += 100000
        ld t0, 0(s2)
        li t1, 100000
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
    mti_srv:
        # disable mie[mtie]
        csrc mie, s0
        # enable mip[stip]
        csrs mip, s1
    trap_m_ret:
    popa
    mret