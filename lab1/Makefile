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

INCLUDE = -I ../include
CF = -O3 -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -ffunction-sections -fdata-sections \
-nostartfiles -nostdlib -nostdinc -static -lgcc -Wl,--nmagic -Wl,--gc-sections
CC := ${GCC}
CFLAGS := ${CF} ${INCLUDE}

ASFLAGS := -march=$(ISA) -mabi=$(ABI)

.PHONY: all run debug clean
all:
	make -C init
	make -C arch/riscv

run: all
	qemu-system-riscv64 -nographic -machine virt -kernel vmlinux

debug: all
	qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -S -s

clean: 
	make -C init clean
	make -C arch/riscv clean