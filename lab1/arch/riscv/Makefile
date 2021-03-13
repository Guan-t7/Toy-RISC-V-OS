VPATH:=${ROOT}/init:kernel

vmlinux: main.o test.o
	make -C kernel
	${LD} -T kernel/vmlinux.lds -o ${ROOT}/vmlinux ${ROOT}/init/main.o ${ROOT}/init/test.o ./kernel/head.o
	${OBJCOPY} -O binary --strip-all ${ROOT}/vmlinux ${ROOT}/arch/riscv/boot/Image

clean:
	-rm ${ROOT}/vmlinux
	-rm ${ROOT}/arch/riscv/boot/Image
	make -C kernel clean