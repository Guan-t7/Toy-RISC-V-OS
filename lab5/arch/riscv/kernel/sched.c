#include "put.h"
#include "rand.h"
#include "sched.h"
#include "stddef.h"
#include "vm.h"

#define PRIORITY

struct task_struct *current;
struct task_struct *task[NR_TASKS];

#ifdef PRIORITY
static const long COUNTER_INIT[] = {0, 7, 6, 5, 4};
#endif

void task_pre_run() __attribute__((noreturn));
/* control led here after `ret` in S-mode task

A U-mode task is to run. This piece of code is for preparation:
* prep PT for U-mode task
* ecall from S-mode informs M-mode the completion of STI processing;
* CSRs[sstatus].SPP = 0b0, CSRs[sstatus].SPIE = 1
* control passed to U-mode task after `sret`
*/
void task_pre_run()
{
    swap_satp();
    asm("csrr s11, sepc;\
         ecall;\
         csrw sepc, s11;\
         csrrw sp, sscratch, sp;\
         csrc sstatus, %0;\
         csrs sstatus, %1;\
         sret;"
        :
        : "r"(0b100000000), "r"(0b10000)
        :);
}

void task_init(void)
{
    // init current
    current = (void *)0xffffffe000f00000;
    //? dummy init task[0]
    task[0] = current;
    task[0]->state = TASK_RUNNING;
    task[0]->counter = 1; //! shouldn't be 0
    task[0]->priority = 5;
    task[0]->blocked = 0;
    task[0]->pid = 0;
    task[0]->thread.ssp = (uint64)task[0] + TASK_SIZE;
    task[0]->mm.satp = get_satp();
    // init task[1-4],
    for (long i = 1; i <= LAB_TEST_NUM; i++)
    {
        task[i] = (void *)((uint64)task[0] + i * TASK_SIZE);
        // as subprocess forked from task[0]
        *task[i] = *task[0]; //! require memcpy
        // diff
        task[i]->pid = i;
        task[i]->thread.ssp = task[i] + TASK_SIZE;
        task[i]->thread.usp = 0xffffffdf80000000;
        task[i]->thread.ra = (uint64)task_pre_run;
        task[i]->thread.sepc = ucode_va;
        uint64 root_ppn = u_paging_init(task[i]) >> 12;
        task[i]->mm.satp = ((uint64)8 << 60) | root_ppn;

#ifdef SJF
        task[i]->counter = rand();
#elif defined PRIORITY
        task[i]->counter = COUNTER_INIT[i];
#endif
        my_printk("[PID = %ld] Process Created Successfully! counter = %ld", i, task[i]->counter);
#ifdef PRIORITY
        my_printk(" priority = %ld", task[i]->priority);
#endif
        putchar('\n');
    }
}

#ifdef SJF
void do_timer(void)
{
    current->counter--;
    my_printk("[PID = %ld] Context Calculation: counter = %ld\n", current->pid, current->counter);
    // PREEMPT_DISABLE
    if (current->counter == 0)
    {
        schedule();
    }
}
#endif

#ifdef PRIORITY
void do_timer(void)
{
    current->counter--;
    if (current->counter == 0)
    {
        current->counter = COUNTER_INIT[current->pid];
    }
    // PREEMPT_ENABLE
    schedule();
}
#endif

void print_change(struct task_struct *t)
{
    my_printk("[PID = %ld] ", t->pid);
#ifdef SJF
    my_printk("Reset counter = %ld", t->counter);
#endif
#ifdef PRIORITY
    my_printk("counter = %ld priority = %ld", t->counter, t->priority);
#endif
    puts("\n");
}

void print_ts(struct task_struct *ts)
{
    my_printk(" [task struct: %lx, ssp: %lx]", (uint64)ts, (uint64)(ts->thread.ssp));
}

void print_switch(struct task_struct *current, struct task_struct *next)
{
    my_printk("[!] Switch from task %ld", current->pid);
    print_ts(current);
    my_printk(" to task %ld", next->pid);
    print_ts(next);
#ifdef PRIORITY
    my_printk(", prio: %ld", next->priority);
#endif
    my_printk(", counter: %ld\n", next->counter);
}

#ifdef SJF
void schedule(void)
{
    long i;
    struct task_struct *t;

    while (1)
    {
        // find i where task[i]->counter != 0
        for (i = LAB_TEST_NUM; i != 0 && task[i]->counter == 0; i--)
            ;
        // found
        if (i != 0)
            break;
        // task[i]->counter == 0 for all i
        // re-assign counter for these tasks
        // direction follows test case
        for (i = 1; i <= LAB_TEST_NUM; i++)
        {
            t = task[i];
            t->counter = rand();
            print_change(t);
        }
        // schedule again
    }

    // find next proc to exec
    for (t = task[i]; i != 0; i--)
    {
        // not finished; shortest job
        if (task[i]->counter != 0 && task[i]->counter < t->counter)
        {
            t = task[i];
        }
    }
    print_switch(current, t);
    switch_to(t);
}
#endif

#ifdef PRIORITY
void schedule(void)
{
    long i;
    struct task_struct *t;
    for (i = LAB_TEST_NUM, t = task[i]; i != 0; i--)
    {
        // the 2 rules
        if (task[i]->priority < t->priority || task[i]->priority == t->priority && task[i]->counter < t->counter)
        {
            t = task[i];
        }
    }
    print_switch(current, t);
    puts("tasks' priority changed\n");
    // task prio random update
    for (i = 1; i <= LAB_TEST_NUM; i++)
    {
        task[i]->priority = rand();
        print_change(task[i]);
    }
    switch_to(t);
}
#endif

void switch_to(struct task_struct *next)
{
    if (next != current)
    {
        //! offset hard coded
        // ssp is in sp, usp is in sscratch
        // save current
        asm(
            "sd ra,  0 (%0);\
            sd sp, 120(%0);\
            sd s0, 16 (%0);\
            sd s1, 24 (%0);\
            sd s2, 32 (%0);\
            sd s3, 40 (%0);\
            sd s4, 48 (%0);\
            sd s5, 56 (%0);\
            sd s6, 64 (%0);\
            sd s7, 72 (%0);\
            sd s8, 80 (%0);\
            sd s9, 88 (%0);\
            sd s10,96 (%0);\
            sd s11,104(%0);\
            csrr s0, sepc;\
            sd s0, 112(%0);\
            csrr s0, sscratch;\
            sd s0,  8 (%0);"
            :
            : "r"(&current->thread)
            : "memory");
        // load next
        asm(
            "ld s1,  8 (%0);\
            csrw sscratch, s1;\
            ld s1, 112(%0);\
            csrw sepc, s1;\
            ld ra,  0 (%0);\
            ld sp, 120(%0);\
            ld s0, 16 (%0);\
            ld s1, 24 (%0);\
            ld s2, 32 (%0);\
            ld s3, 40 (%0);\
            ld s4, 48 (%0);\
            ld s5, 56 (%0);\
            ld s6, 64 (%0);\
            ld s7, 72 (%0);\
            ld s8, 80 (%0);\
            ld s9, 88 (%0);\
            ld s10,96 (%0);\
            ld s11,104(%0);"
            :
            : "r"(&next->thread)
            :); //! gcc shouldn't know this
        current = next;
    }
}