#ifndef STACK_H
#define STACK_H

typedef int elem_t;
#define elemFormat "%d"

const size_t DEFAULT_CAPACITY = 8;
const size_t LAST_PRINTED     = 16;
const size_t ELEM_PRINT_ADD   = 4;
const size_t REALLOC_RATE     = 2;

const unsigned int CANARY = 0xDEADBEEF;

struct stack
{
    unsigned int stackCanary1;
    elem_t *data;

    size_t capacity;
    size_t size;

    unsigned int stackCanary2;
};

enum stackErrorEnum
{
    STACK_OK       = 0,
    STACK_NULL     = 1,
    DATA_NULL      = 2,
    SMALL_CAPACITY = 3,
    ANTI_OVERFLOW  = 4,
    REALLOC_FAILED = 5,
    CHANGED_CANARY = 6
};

stackErrorEnum stackError(stack *stk);

stackErrorEnum stackCtor(stack *stk, size_t capacity);

stackErrorEnum stackDtor(stack *stk);

stackErrorEnum stackDump(stack *stk, const char *file, int line, const char *function);

#define STACK_DUMP(stk) stackDump((stk), __FILE__, __LINE__, __func__);

stackErrorEnum stackPush(stack *stk, elem_t value);

stackErrorEnum stackPop(stack *stk, elem_t *returnValue);

stackErrorEnum stackRealloc(stack *stk);

#endif //STACK_H
