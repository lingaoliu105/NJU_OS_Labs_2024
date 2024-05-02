#include "co.h"
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include "PriorityQueue.h"
#include <time.h>
#include <string.h>

unsigned long g_clock = 0ul;
struct co main_co = {"main", NULL, NULL, 0ul, CO_RUNNING, NULL};
struct co *current = &main_co; // global pointer to the current executing coroutine

// 用一个全局容器保存所有coroutine
// 假设最多100个协程 (defined in PriorityQueue.h)
// 使用pq根据最后执行时间进行调度,没有采用随机方案
PriorityQueue co_list;

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
#else
        "movl %0, %%esp; movl %2, 4(%0);movl %3, 8(%0); jmp *%1"
        :
        : "b"((uintptr_t)sp - 8),
          "d"(entry),
          "a"(arg1),
          "c"(arg2)
        : "memory"
#endif
    );
}

void wrapped_task(void (*func)(void *), void *arg)
{
    current->status = CO_RUNNING;
    func(arg);
    current->status = CO_DEAD;
    co_yield (); // 等到下次yield到waiter时回收
}

struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
    // construct co
    struct co *co_ptr = (struct co *)malloc(sizeof(struct co));
    co_ptr->name = name;
    co_ptr->func = func;
    co_ptr->arg = arg;

    g_clock++;
    co_ptr->last_execution_time = g_clock;

    co_ptr->status = CO_NEW;

    // register co in the co list
    push(&co_list, co_ptr);

    return co_ptr;
}

void co_wait(struct co *co)
{
    // keep yielding to other coroutine until co is dead
    current->status = CO_WAITING;
    co->waiter = current;
    while (co->status != CO_DEAD)
    {
        co_yield ();
    }
    free(co); // 在此处回收资源,不在yield中
    co = NULL;
    current->status = CO_RUNNING;
}

void co_yield ()
{
    int val = setjmp(current->jump_buffer);
    if (val == 0)
    {
        // yielding to other co
        g_clock++;
        current->last_execution_time = g_clock;
        struct co *next = pop(&co_list);
        if (current->status != CO_DEAD)
        {
            push(&co_list, current);
        }
        current = next;
        if (current == NULL)
        {
            perror("cannot find next task\n"); // not supposed to happen
            return;
        }
        if (current->status == CO_NEW)
        {
            stack_switch_call_biarg(&current->stack[STACK_SIZE], wrapped_task, (uintptr_t)current->func, (uintptr_t)current->arg);
        }
        else if (current->status == CO_RUNNING)
        {
            longjmp(current->jump_buffer, 0);
        }
        else if (current->status == CO_WAITING)
        {
            longjmp(current->jump_buffer, 0); // 由waiting coroutine的wait()处理(在彼处yield), 此处就不换next了
        }
        // co_dead not supposed to appear
    }
    else
    {
        // some other coroutine called yield. just return
        return;
    }
}
