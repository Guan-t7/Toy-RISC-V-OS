#ifndef SCHED_H
#define SCHED_H

#include "mm_types.h"

#define TASK_SIZE (4096)
#define THREAD_OFFSET (5 * 0x08)

#ifndef __ASSEMBLER__

/* task的最大数量 */
#define NR_TASKS 64

#define FIRST_TASK (task[0])
#define LAST_TASK (task[NR_TASKS - 1])

/* 定义task的状态，Lab3中task只需要一种状态。*/
#define TASK_RUNNING 0
// #define TASK_INTERRUPTIBLE       1
// #define TASK_UNINTERRUPTIBLE     2
// #define TASK_ZOMBIE              3
// #define TASK_STOPPED             4

#define PREEMPT_ENABLE 0
#define PREEMPT_DISABLE 1

/* Lab3中进程的数量以及每个进程初始的时间片 */
#define LAB_TEST_NUM 4
#define LAB_TEST_COUNTER 5

/* 当前进程 */
extern struct task_struct *current;

/* 进程指针数组 */
extern struct task_struct *task[NR_TASKS];

/* 进程状态段数据结构 */
struct thread_struct
{
  uint64 ra;
  uint64 usp;
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
  uint64 sepc;
  uint64 ssp;
};

/* 进程数据结构 */
struct task_struct
{
  long state;    // 进程状态 Lab3中进程初始化时置为TASK_RUNNING
  long counter;  // 运行剩余时间
  long priority; // 运行优先级 1最高 5最低
  long blocked;
  long pid; // 进程标识符
  // Above Size Cost: 5 * 8 bytes
  struct thread_struct thread; // 该进程状态段
  struct mm_struct mm;
};

/* 进程初始化 创建四个_进程 */
void task_init(void);

/* 在时钟中断处理中被调用 */
void do_timer(void);

/* 调度程序 */
void schedule(void);

/* 切换当前任务current到下一个任务next */
void switch_to(struct task_struct *next);

#endif

#endif