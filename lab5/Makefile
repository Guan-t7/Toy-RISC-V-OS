export
ROOT = $(shell pwd)
CROSS_= riscv64-unknown-elf-
AR=${CROSS_}ar
AS=${CROSS_}as
GCC=${CROSS_}gcc
LD=${CROSS_}ld
OBJCOPY=${CROSS_}objcopy

ISA ?= rv64imafd
ABI ?= lp64

INCLUDE = -I ../include -I ${ROOT}/include
CF = -Og -g -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -ffunction-sections -fdata-sections \
-nostartfiles -nostdlib -nostdinc -static -lgcc -Wl,--nmagic -Wl,--gc-sections
CC := ${GCC}
# -D
CFLAGS := ${CF} ${INCLUDE}

ASFLAGS := ${INCLUDE} -march=$(ISA) -mabi=$(ABI)

.PHONY: all run debug clean
all:
	make -C init
	make -C lib
	make -C arch/riscv
	${LD} -T arch/riscv/kernel/vmlinux.lds -o vmlinux init/main.o init/test.o lib/put.o lib/rand.o arch/riscv/kernel/head.o arch/riscv/kernel/entry.o arch/riscv/kernel/mtrap.o arch/riscv/kernel/strap.o arch/riscv/kernel/sched.o arch/riscv/kernel/vm.o arch/riscv/kernel/memcpy.o arch/riscv/kernel/memset.o

run: all
	# qemu-system-riscv64 -nographic -machine virt -kernel vmlinux
	qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -initrd hello.bin
debug: all
	# qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -S -s
	qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -initrd hello.bin -S -s
clean: 
	-rm vmlinux
	make -C init clean
	make -C lib clean
	make -C arch/riscv clean