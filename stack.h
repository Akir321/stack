#ifndef STACK_H
#define STACK_H

typedef int elem_t;
#define elemFormat "%d"

const size_t DEFAULT_CAPACITY = 8;
const size_t LAST_PRINTED = 8;
const size_t ELEM_PRINT_ADD = 4;

struct stack
{
    elem_t *data;

    size_t capacity;
    size_t size;
};

enum stackErrorEnum
{
    STACK_OK       = 0,
    STACK_NULL     = 1,
    DATA_NULL      = 2,
    SMALL_CAPACITY = 4
};

stackErrorEnum stackError(stack *stk);

stackErrorEnum stackCtor(stack *stk, size_t capacity);

stackErrorEnum stackDtor(stack *stk);

stackErrorEnum stackDump(stack *stk, const char *file, int line, const char *function);

stackErrorEnum stackPush(stack *stk, elem_t value);

#endif //STACK_H
