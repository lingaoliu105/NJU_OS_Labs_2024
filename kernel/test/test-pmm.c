#include <common.h>
#include <klib.h>

#define MAX_PTR_NUM 300
enum ops
{
    OP_ALLOC = 1,
    OP_FREE
};

void * alloced_ptrs[MAX_PTR_NUM];
int is_free[MAX_PTR_NUM]={0};

struct malloc_op
{
    enum ops type;
    union
    {
        size_t sz;  // OP_ALLOC: size
        void *addr; // OP_FREE: address
    };
};

struct malloc_op random_op(){
generate:
    srand(rand());
    struct malloc_op operation;
    int op = rand()%2;
    if (op==OP_ALLOC){
        operation.sz = rand()%30000;
    }else{
        int index = rand()%MAX_PTR_NUM;
        if (is_free[index]){
            goto generate;
        }else{
            operation.addr = alloced_ptrs[index];
        }
    }
    return operation;
}

void alloc_check(void * ptr, int size){
    char* converted_ptr = (char*) ptr;
    for (int i =0;i<size;i++){
        // check if the memory is writable
        converted_ptr[i] = '0';
    }
    for (int i = 0; i < size; i++)
    {
        // check if the memory is writable
        assert (converted_ptr[i] == '0');
    }
}

void stress_test()
{
    while (1)
    {
        // 根据 workload 生成操作
        struct malloc_op op = random_op();

        switch (op.type)
        {
        case OP_ALLOC:
        {
            void *ptr = pmm->alloc(op.sz);
            alloc_check(ptr, op.sz);
            break;
        }
        case OP_FREE:
            free(op.addr);
            break;
        }
    }
}

int main()
{
    os->init();
    mpe_init(stress_test);
}