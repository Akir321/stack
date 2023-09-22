#include <assert.h>
#include <stdio.h>

#include "stack.h"

#define STACK_VERIFY stackErrorEnum error = stackError(stk); \
if (error) \
{ \
    printf("STACK CORRUPTED\n"); \
    STACK_DUMP(stk); \
    return error; \
}

stackErrorEnum stackError(stack *stk)
{
    if (!stk)                            return STACK_NULL;
    if (!stk->data)                      return DATA_NULL;
    if (stk->capacity < stk->size)       return SMALL_CAPACITY;
    if (stk->stackCanary1 != STK_CANARY) return CHANGED_CANARY;
    if (stk->stackCanary2 != STK_CANARY) return CHANGED_CANARY;
    unsigned int buf_canary = *((unsigned int *)stk->data - 1);
    if (buf_canary != BUF_CANARY)        return CHANGED_CANARY;
    buf_canary = *(unsigned int *)(stk->data + stk->capacity);
    if (buf_canary != BUF_CANARY)        return CHANGED_CANARY;

    return STACK_OK;
}

stackErrorEnum stackCtor(stack *stk, size_t capacity)
{
    assert(stk);

    stk->capacity = (capacity > 0) ? capacity : DEFAULT_CAPACITY;
    stk->size = 0;

    stk->stackCanary1 = STK_CANARY;
    stk->stackCanary2 = STK_CANARY;

    stk->data = (elem_t *)myCalloc(stk->capacity, sizeof(elem_t));
    //printf("data[%p], capacity = %lld\n", stk->data, stk->capacity);
    if (!stk->data) return DATA_NULL;

    *(unsigned int *) stk->data                  = BUF_CANARY;
    stk->data = (elem_t *)((int *)stk->data + 1);
    *(unsigned int *)(stk->data + stk->capacity) = BUF_CANARY;

    return STACK_OK;
}

stackErrorEnum stackDtor(stack *stk)
{
    assert(stk);
    
    stk->data = (elem_t *)((int *)stk->data - 1);
    if (stk->data) free(stk->data);
    stk->data = NULL;
    
    stk->capacity = 0;
    stk->size = -1;

    stk->stackCanary1 = 0;
    stk->stackCanary2 = 0;

    return STACK_OK;
}

stackErrorEnum stackDump(stack *stk, const char *file, int line, const char *function)
{
    assert(stk);
    printf("I'm stackDump called from %s (%d) %s\n", function, line, file);
    printf(" capacity = %lld\n size = %lld\n", stk->capacity, stk->size);

    printf("stackCanary1  = 0x%x\n", stk->stackCanary1);
    printf("stackCanary2  = 0x%x\n", stk->stackCanary2);

    printf(" data[%p] = ", stk->data);
    if (!stk->data)
    {
        printf("NULL\n");
        return DATA_NULL;
    }
    putchar('\n');

    for (size_t i = 0; 
         i < stk->size + ELEM_PRINT_ADD 
         && i < stk->capacity
         && i < LAST_PRINTED; i++)
    {
        printf("   ");
        if (i < stk->size) putchar('*');
        else               putchar(' ');
        printf("[%lld] = " elemFormat "\n", i, stk->data[i]);
    }

    unsigned int buf_canary = *((unsigned int *)stk->data - 1);
    printf("bufferCanary1 = 0x%x\n", buf_canary);
    buf_canary = *(unsigned int *)(stk->data + stk->capacity);
    printf("bufferCanary2 = 0x%x\n", buf_canary);

    return stackError(stk);
}

stackErrorEnum stackPush(stack *stk, elem_t value)
{
    STACK_VERIFY;

    if (stk->size >= stk->capacity) stackRealloc(stk);

    stk->data[stk->size++] = value;
    return STACK_OK;
}

stackErrorEnum stackPop(stack *stk, elem_t *returnValue)
{
    STACK_VERIFY;
    assert(returnValue);

    if (!stk->size) return ANTI_OVERFLOW;
    if (stk->size * REALLOC_RATE  * REALLOC_RATE <= stk->capacity) stackRealloc(stk);

    *returnValue = stk->data[--stk->size];
    //stk->data[stk->size] = 0;

    return STACK_OK;
}

stackErrorEnum stackRealloc(stack *stk)
{
    STACK_VERIFY;

    printf("i'm stackRealloc\n");
    printf("capacity = %lld\nsize = %lld\n", stk->capacity, stk->size);
    printf("start reallocing\n");

    stk->data = (elem_t *)((int *)stk->data - 1);
    size_t prevCapacity = stk->capacity;

    if (stk->size >= stk->capacity)
    {
        stk->capacity *= REALLOC_RATE;
        stk->data = (elem_t *)realloc(stk->data, stk->capacity * sizeof(elem_t) + 2 * sizeof(int));
        if (!stk->data) return REALLOC_FAILED;
    }
    else if (stk->size * REALLOC_RATE  * REALLOC_RATE <= stk->capacity)
    {
        stk->capacity /= REALLOC_RATE;
        stk->data = (elem_t *)realloc(stk->data, stk->capacity * sizeof(elem_t) + 2 * sizeof(int));
        if (!stk->data) return REALLOC_FAILED;
    }
    stk->data = (int *)stk->data + 1;
    *(unsigned int *)(stk->data + prevCapacity)  = 0;
    *(unsigned int *)(stk->data + stk->capacity) = BUF_CANARY;

    printf("realloc finished\n");
    printf("new capacity = %lld\nnew size = %lld\n", stk->capacity, stk->size);
    return STACK_OK;
}

void *myCalloc(size_t elementNum, size_t elementSize)
{
    size_t bufSize = elementSize * elementNum + 2 * sizeof(int);
    void *buffer = malloc(bufSize);
    if (!buffer) return NULL;

    char *first = (char *)buffer;
    char *last  = (char *)buffer + bufSize;
    for ( ; first != last; first++)
    {
        *first = 0;
    }
    
    return buffer;
}
