.globl trap_s

.section .text.entry

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

# tracking from here. Maintain stack balance!
trap_s:
#! sp unchanged
# push GPR
pusha
# push sepc
csrr a0,sepc
addi sp,sp,-8
sd a0,0(sp)

jal do_timer
ecall
# pop sepc
ld a0,0(sp)
addi sp,sp,8
csrw sepc,a0
# pop GPR
popa
sret