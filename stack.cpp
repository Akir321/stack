#include <assert.h>
#include <stdio.h>

#include "stack.h"

stackErrorEnum stackError(stack *stk)
{
    if (!stk)                      return STACK_NULL;
    if (!stk->data)                return DATA_NULL;
    if (stk->capacity < stk->size) return SMALL_CAPACITY;

    return STACK_OK;
}

stackErrorEnum stackCtor(stack *stk, size_t capacity)
{
    assert(stk);

    stk->capacity = (capacity > 0) ? capacity : DEFAULT_CAPACITY;
    stk->size = 0;

    stk->data = (elem_t *)calloc(stk->capacity, sizeof(elem_t));
    printf("data[%p], capacity = %lld\n", stk->data, stk->capacity);
    if (!stk->data) return DATA_NULL;

    return STACK_OK;
}

stackErrorEnum stackDtor(stack *stk)
{
    assert(stk);
    
    if (stk->data) free(stk->data);
    stk->data = NULL;
    
    stk->capacity = 0;
    stk->size = -1;

    return STACK_OK;
}

stackErrorEnum stackPush(stack *stk, elem_t value)
{
    stackErrorEnum error = stackError(stk);
    if (error) return error;

    if (stk->size == stk->capacity) return SMALL_CAPACITY;

    stk->data[stk->size++] = value;
    return STACK_OK;
}
