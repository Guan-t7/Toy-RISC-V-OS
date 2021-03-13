.globl _start

.text

_start:
# We start in M mode
# CSRR* to cope with control status register
csrci mstatus, 0b01000 # Reset MIE in CSRs[mstatus] to disable interrupt
la t0, _start
csrw mtvec, t0 # 设置machine mode异常处理地址
la a0, S_start
csrw mepc, a0 # Supervisor mode start address
# CSRs[mstatus].MPP = 0b01, CSRs[mstatus].MPIE = 0
li a1, 0b01100010000000
li a2, 0b00100000000000
csrc mstatus, a1
csrs mstatus, a2
mret # to supervisor mode

S_start:
csrw stvec, t0 # 设置supervisor mode异常处理地址
la sp, stack_top # 栈顶指针
j start_kernel