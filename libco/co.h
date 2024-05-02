#define STACK_SIZE (1024 * 1024)
#include <time.h>
#include <setjmp.h>
#include <stdint.h>
#ifndef CO_H
#define CO_H

// 将co_status和co定义移至头文件以便priority queue代码使用
enum co_status
{
    CO_NEW = 1, // 新创建，还未执行过
    CO_RUNNING, // 已经执行过
    CO_WAITING, // 在 co_wait 上等待. not really used in current implementation
    CO_DEAD,    // 已经结束，但还未释放资源
};

struct co
{
    const char *name;
    void (*func)(void *); // co_start 指定的入口地址和参数. 名为func的函数指针
    void *arg;

    unsigned long last_execution_time; // timestamp for last execution. used as criteria for scheduling.
    enum co_status status;             // 协程的状态
    struct co *waiter;                 // 是否有其他协程在等待当前协程. not really used in current implementation
    jmp_buf jump_buffer;               // 寄存器现场. 通过setjmp库的jump_buffer表示
    uint8_t stack[STACK_SIZE];         // 协程的堆栈
};

struct co *co_start(const char *name, void (*func)(void *), void *arg);
void co_yield ();
void co_wait(struct co *co);
#endif