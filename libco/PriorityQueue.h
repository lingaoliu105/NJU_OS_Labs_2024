#include "co.h" // 确保这个头文件里有struct co的定义，并且有last_executed成员
#include <stdio.h>

#define MAX_SIZE 100

typedef struct
{
    struct co* co_arr[MAX_SIZE]; // 修改为指向co结构体的指针
    int size;
} PriorityQueue;

// 使用co->last_executed进行比较
#define LESS(a, b) ((a)->last_execution_time <= (b)->last_execution_time)

void swap(struct co** a, struct co** b)
{
    struct co* temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(PriorityQueue *pq, int index)
{
    if (index <= 0)
        return;

    int parent = (index - 1) / 2;
    if (LESS(pq->co_arr[parent], pq->co_arr[index]))
    {
        swap(&pq->co_arr[parent], &pq->co_arr[index]);
        heapifyUp(pq, parent);
    }
}

void heapifyDown(PriorityQueue *pq, int index)
{
    int smallest = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;

    if (leftChild < pq->size && LESS(pq->co_arr[leftChild], pq->co_arr[smallest]))
    {
        smallest = leftChild;
    }

    if (rightChild < pq->size && LESS(pq->co_arr[rightChild], pq->co_arr[smallest]))
    {
        smallest = rightChild;
    }

    if (smallest != index)
    {
        swap(&pq->co_arr[index], &pq->co_arr[smallest]);
        heapifyDown(pq, smallest);
    }
}

void push(PriorityQueue *pq, struct co* value)
{
    if (pq->size >= MAX_SIZE)
    {
        printf("Priority queue is full.\n");
        return;
    }
    pq->co_arr[pq->size] = value;
    pq->size++;
    heapifyUp(pq, pq->size - 1);
}

struct co* pop(PriorityQueue *pq)
{
    if (pq->size <= 0)
    {
        printf("Priority queue is empty.\n");
        return NULL;
    }
    struct co* top = pq->co_arr[0];
    pq->co_arr[0] = pq->co_arr[pq->size - 1];
    pq->size--;
    heapifyDown(pq, 0);
    return top;
}