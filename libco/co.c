#include "co.h"
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include "PriorityQueue.h"
#include <time.h>
#include <string.h>

unsigned long g_clock = 0ul;
struct co main_co = {"main",NULL,NULL,0ul,CO_RUNNING,NULL};
struct co *current = &main_co; // global pointer to the current executing coroutine


// 先设想我们需要用一个全局容器保存所有coroutine
// 假设最多100个协程 (defined in PriorityQueue.h)
PriorityQueue co_list;

// struct context {
//     uint64_t rax;      // 通用寄存器
//     uint64_t rbx;      // 通用寄存器
//     uint64_t rcx;      // 通用寄存器，常用于循环计数等
//     uint64_t rdx;      // 通用寄存器，常用于I/O操作
//     uint64_t rsi;      // 通用寄存器，常用于系统调用参数
//     uint64_t rdi;      // 通用寄存器，常用于系统调用参数
//     uint64_t rbp;      // 基址指针寄存器，用于栈帧定位
//     uint64_t rsp;      // 堆栈指针寄存器，指向当前栈顶
//     uint64_t r8;       // 扩展通用寄存器
//     uint64_t r9;       // 扩展通用寄存器
//     uint64_t r10;      // 扩展通用寄存器
//     uint64_t r11;      // 扩展通用寄存器，有时用于临时存储
//     uint64_t r12;      // 扩展通用寄存器，可用于线程本地存储
//     uint64_t r13;      // 扩展通用寄存器，常用于存储进程/线程的上下文信息（如栈指针）
//     uint64_t r14;      // 扩展通用寄存器，常用于存储下一个任务或线程的上下文
//     uint64_t r15;      // 扩展通用寄存器

//     // 控制和状态寄存器（部分可能不适合直接保存在结构体中，因为它们包含系统级状态）
//     // 下面的寄存器通常通过特定指令访问，而不是直接在用户空间保存其值
//     // 但为了完整性列出：
//     uint64_t rip;     // 指令指针寄存器，指出下一条指令地址
//     // uint64_t rflags;  // 标志寄存器，包含CPU状态标志
//     // uint64_t cs;      // 代码段选择子
//     // uint64_t ss;      // 堆栈段选择子
//     // uint64_t fs;      // 辅助段选择子
//     // uint64_t gs;      // 辅助段选择子

//     // 其他可能需要保存的系统相关状态（根据具体应用场景决定是否需要）
//     // 例如：浮点状态寄存器集合（fxsave/fxrstor指令操作）、调试寄存器等
// };

jmp_buf main_buffer;


static inline void
stack_switch_call(void *sp, void *entry, uintptr_t arg)
{
    asm volatile(
#if __x86_64__
        "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
        :
        : "b"((uintptr_t)sp),
          "d"(entry),
          "a"(arg)
        : "memory"
#else
        "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
        :
        : "b"((uintptr_t)sp - 8),
          "d"(entry),
          "a"(arg)
        : "memory"
#endif
    );
}
static inline void
stack_switch_call_biarg(void *sp, void *entry, uintptr_t arg1, uintptr_t arg2)
{
    asm volatile(
#if __x86_64__
        "movq %0, %%rsp; movq %2, %%rdi; movq %3, %%rsi; jmp *%1"
        :
        : "b"((uintptr_t)sp),
          "d"(entry),
          "a"(arg1),
          "c"(arg2)
        : "memory"
#else // TODO: complete implementation
        "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
        :
        : "b"((uintptr_t)sp - 8),
          "d"(entry),
          "a"(arg1)
        : "memory"
#endif
    );
}

void wrapped_task(void (*func)(void *), void *arg)
{
    current->status = CO_RUNNING;
    func(arg);
    printf("finish task\n");
    current->status = CO_DEAD;
    co_yield(); // 等到下次yield到waiter时回收
}

struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
    // construct co
    printf("costart\n");
    struct co *co_ptr = (struct co *)malloc(sizeof(struct co));
    co_ptr->name = name;
    co_ptr->func = func;
    co_ptr->arg = arg;
    // time(&co_ptr->last_execution_time);
    g_clock++;
    co_ptr->last_execution_time = g_clock;
    
    co_ptr->status = CO_NEW;
    // register co in the co list
    push(&co_list, co_ptr); // take note of the timestamp
    
    return co_ptr;
}

void co_wait(struct co *co)
{
    // 若current为空,此时无任务,则可开始执行func
    // if (current == NULL)
    // {
    //     co_yield();
    //     co->status = CO_RUNNING;
    //     current = co;

    //     int jmp_result = setjmp(main_buffer); // 给main留一个buffer.假如没有其他协程则跳转回来
    //     if (jmp_result == 0)
    //     {
    //         stack_switch_call_biarg(&current->stack[STACK_SIZE], wrapped_task, (uintptr_t)current->func, (uintptr_t)arg);
    //     } // else return
    // }
    // block parent thread until co is dead
    current->status = CO_WAITING;
    co->waiter = current;
    while (co->status != CO_DEAD)
    {
        co_yield();
    }
    free(co); // 在此处回收资源,不在yield中
    co = NULL;
    current->status = CO_RUNNING;
}

void co_yield ()
{
    // printf("yield1\n");
    int val = setjmp(current->jump_buffer);
    if (val == 0)
    {
        // yielding to other co
        // time(&current->last_execution_time);
        g_clock++;
        current->last_execution_time = g_clock;
        struct co *next = pop(&co_list);
        if (current->status!=CO_DEAD){
            push(&co_list, current);
        }
        printf("current: %s, next: %s\n", current->name, next->name);
        current = next;
        if (current == NULL)
        {
            printf("cannot find next task\n"); // not supposed to happen
            return;
        }
        if (current->status == CO_NEW)
        {
            // stack_switch_call(&current->stack[STACK_SIZE], current->func, (uintptr_t)current->arg);
            stack_switch_call_biarg(&current->stack[STACK_SIZE], wrapped_task, (uintptr_t)current->func, (uintptr_t) current->arg);
        }
        else if (current->status == CO_RUNNING)
        {
            longjmp(current->jump_buffer, 0);
        }
        else if (current->status==CO_WAITING){
            longjmp(current->jump_buffer, 0); // 由waiting coroutine的wait()处理
        }
        // co_dead not supposed to appear
    }
    else
    {
        // some other coroutine called yield. just return
        return;
    }
}
