.macro pusha
    addi sp,sp,-30*8
    sd x1, 0*8 (sp)
    sd x3, 1*8 (sp)
    sd x4, 2*8 (sp)
    sd x5, 3*8 (sp)
    sd x6, 4*8 (sp)
    sd x7, 5*8 (sp)
    sd x8, 6*8 (sp)
    sd x9, 7*8 (sp)
    sd x10,8*8 (sp)
    sd x11,9*8 (sp)
    sd x12,10*8(sp)
    sd x13,11*8(sp)
    sd x14,12*8(sp)
    sd x15,13*8(sp)
    sd x16,14*8(sp)
    sd x17,15*8(sp)
    sd x18,16*8(sp)
    sd x19,17*8(sp)
    sd x20,18*8(sp)
    sd x21,19*8(sp)
    sd x22,20*8(sp)
    sd x23,21*8(sp)
    sd x24,22*8(sp)
    sd x25,23*8(sp)
    sd x26,24*8(sp)
    sd x27,25*8(sp)
    sd x28,26*8(sp)
    sd x29,27*8(sp)
    sd x30,28*8(sp)
    sd x31,29*8(sp)
.endm

.macro popa
    ld x1, 0*8 (sp)
    ld x3, 1*8 (sp)
    ld x4, 2*8 (sp)
    ld x5, 3*8 (sp)
    ld x6, 4*8 (sp)
    ld x7, 5*8 (sp)
    ld x8, 6*8 (sp)
    ld x9, 7*8 (sp)
    ld x10,8*8 (sp)
    ld x11,9*8 (sp)
    ld x12,10*8(sp)
    ld x13,11*8(sp)
    ld x14,12*8(sp)
    ld x15,13*8(sp)
    ld x16,14*8(sp)
    ld x17,15*8(sp)
    ld x18,16*8(sp)
    ld x19,17*8(sp)
    ld x20,18*8(sp)
    ld x21,19*8(sp)
    ld x22,20*8(sp)
    ld x23,21*8(sp)
    ld x24,22*8(sp)
    ld x25,23*8(sp)
    ld x26,24*8(sp)
    ld x27,25*8(sp)
    ld x28,26*8(sp)
    ld x29,27*8(sp)
    ld x30,28*8(sp)
    ld x31,29*8(sp)
    addi sp,sp,30*8
.endm