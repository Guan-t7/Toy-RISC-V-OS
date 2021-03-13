#include "test.h"
#include "put.h"


void enable_interrupt()
{
	// Set CSRs[sstatus].SIE
	asm("csrsi sstatus, 0b00010" ::
			:);
}

int start_kernel()
{
	my_printk("ZJU OS LAB 5             3180103008\n");
	task_init();
	enable_interrupt();
	os_test();

	shutdown();
	return 0;
}