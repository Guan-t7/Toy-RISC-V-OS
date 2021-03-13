#include "sched.h"
#include "rand.h"
#include "put.h"

struct task_struct *current;
struct task_struct *task[NR_TASKS];

#ifdef PRIORITY
static const long COUNTER_INIT[] = {0, 7, 6, 5, 4};
#endif

void task_epc_init()
{
    // led control here with RET;
    asm("csrw sepc, %0;\
        ecall;\
        sret;"
        :
        : "r"(dead_loop)
        :);
}

void task_init(void)
{
    // init current
    current = (void *)0x80010000;
    // init task[0]
    task[0] = current;
    task[0]->state = TASK_RUNNING;
    task[0]->counter = 1; //! shouldn't be 0
    task[0]->priority = 5;
    task[0]->blocked = 0;
    task[0]->pid = 0;
    task[0]->thread.sp = (unsigned long)task[0] + TASK_SIZE;
    // init task[1-4],
    for (long i = 1; i <= LAB_TEST_NUM; i++)
    {
        task[i] = (void *)((unsigned long)task[0] + i * TASK_SIZE);
        // as subprocess forked from task[0]
        *task[i] = *task[0]; //! require memcpy
        // diff
        task[i]->pid = i;
        task[i]->thread.sp = (unsigned long)task[i] + TASK_SIZE;
        task[i]->thread.ra = (unsigned long)task_epc_init;

#ifdef SJF
        task[i]->counter = rand();
#elif defined PRIORITY
        task[i]->counter = COUNTER_INIT[i];
#endif

        puts("[PID = ");
        puti(i);
        puts("] Process Created Successfully! counter = ");
        puti(task[i]->counter);
#ifdef PRIORITY
        puts(" priority = ");
        puti(task[i]->priority);
#endif
        puts("\n");
    }
}

#ifdef SJF
void do_timer(void)
{
    current->counter--;
    puts("[PID = ");
    puti(current->pid);
    puts("] Context Calculation: counter = ");
    puti(current->counter);
    puts("\n");
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
    puts("[PID = ");
    puti(t->pid);
#ifdef SJF
    puts("] Reset counter = ");
    puti(t->counter);
#endif
#ifdef PRIORITY
    puts("] counter = ");
    puti(t->counter);
    puts(" priority = ");
    puti(t->priority);
#endif
    puts("\n");
} 

void print_switch(struct task_struct *current, struct task_struct *next)
{
    puts("[!] Switch from task ");
    puti(current->pid);
    puts(" to task ");
    puti(next->pid);
#ifdef PRIORITY
    puts(", prio: ");
    puti(next->priority);
#endif
    puts(", counter: ");
    puti(next->counter);
    puts("\n");
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
        // save current
        asm("sd ra, 0(%0);\
            sd sp, 8(%0);\
            sd s0,16(%0);\
            sd s1,24(%0);\
            sd s2,32(%0);\
            sd s3,40(%0);\
            sd s4,48(%0);\
            sd s5,56(%0);\
            sd s6,64(%0);\
            sd s7,72(%0);\
            sd s8,80(%0);\
            sd s9,88(%0);\
            sd s10,96(%0);\
            sd s11,104(%0);"
            :
            : "r"(&current->thread)
            : "memory");
        // load next
        asm("ld ra, 0(%0);\
            ld sp, 8(%0);\
            ld s0,16(%0);\
            ld s1,24(%0);\
            ld s2,32(%0);\
            ld s3,40(%0);\
            ld s4,48(%0);\
            ld s5,56(%0);\
            ld s6,64(%0);\
            ld s7,72(%0);\
            ld s8,80(%0);\
            ld s9,88(%0);\
            ld s10,96(%0);\
            ld s11,104(%0);"
            :
            : "r"(&next->thread)
            :); //! gcc shouldn't know this
        //! 环境恶劣
        current = next;
    }
}

void dead_loop(void)
{
    while (1)
        ;
}