#include <assert.h>
#include <stdio.h>

#include "stack.h"

#define STACK_VERIFY \
if (errorFieldToU(stackError(stk))) \
{ \
    printf("STACK CORRUPTED\n"); \
    STACK_DUMP(stk); \
    return stackError(stk); \
}

stackErrorField stackError(stack *stk)
{
    stackErrorField error = {};

    if (!stk)                                { error.stack_null     = 1;  return error; }
    if (!stk->data)                            error.data_null      = 1;
    if (stk->capacity < stk->size)             error.small_capacity = 1;
    if (error.data_null == 1)                  return error;

    if (stk->stackCanary1 != STK_CANARY)       error.changed_canary = 1;
    if (stk->stackCanary2 != STK_CANARY)       error.changed_canary = 1;

    unsigned int buf_canary = *((unsigned int *)stk->data - 1);
    if (buf_canary != BUF_CANARY)              error.changed_canary = 1;
    buf_canary = *(unsigned int *)(stk->data + stk->capacity);
    if (buf_canary != BUF_CANARY)              error.changed_canary = 1;

    if (stackHashCheck(stk).changed_hash == 1) error.changed_hash   = 1;

    return error;
}

unsigned errorFieldToU(stackErrorField error)
{
    return *(unsigned *)&error;
}

stackErrorField stackCtor(stack *stk, size_t capacity)
{
    assert(stk);
    stackErrorField error = {};

    stk->capacity = (capacity > 0) ? capacity : DEFAULT_CAPACITY;
    stk->size = 0;

    stk->stackCanary1 = STK_CANARY;
    stk->stackCanary2 = STK_CANARY;

    stk->data = (elem_t *)myCalloc(stk->capacity, sizeof(elem_t));
    //printf("data[%p], capacity = %lld\n", stk->data, stk->capacity);
    if (!stk->data) 
    {
        error.data_null = 1;
        return error;
    }

    *(unsigned int *) stk->data                  = BUF_CANARY;
    stk->data = (elem_t *)((int *)stk->data + 1);
    *(unsigned int *)(stk->data + stk->capacity) = BUF_CANARY;

    stk->hash = stackHashCalc(stk);
    printf("stackHashCalc = %u\n", stackHashCalc(stk));
    printf("stkHash = %u\n", stk->hash);

    return error;
}

stackErrorField stackDtor(stack *stk)
{
    assert(stk);
    stackErrorField error = {};
    
    stk->data = (elem_t *)((int *)stk->data - 1);
    if (stk->data) free(stk->data);
    stk->data = NULL;
    
    stk->capacity = 0;
    stk->size = -1;

    stk->stackCanary1 = 0;
    stk->stackCanary2 = 0;

    stk->hash = 0;

    return error;
}

stackErrorField stackDump(stack *stk, const char *file, int line, const char *function)
{
    assert(stk);
    printf("I'm stackDump called from %s (%d) %s\n", function, line, file);
    printf(" capacity = %lld\n size = %lld\n", stk->capacity, stk->size);
    printf(" hash = %u\n", stk->hash);

    printf("stackCanary1  = 0x%x\n", stk->stackCanary1);
    printf("stackCanary2  = 0x%x\n", stk->stackCanary2);

    printf(" data[%p] = ", stk->data);
    if (!stk->data)
    {
        printf("NULL\n");
        return stackError(stk);
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

stackErrorField stackPush(stack *stk, elem_t value)
{
    STACK_VERIFY;
    stackErrorField error = {};

    if (stk->size >= stk->capacity) stackRealloc(stk);

    stk->data[stk->size++] = value;

    stk->hash = stackHashCalc(stk);
    return error;
}

stackErrorField stackPop(stack *stk, elem_t *returnValue)
{
    STACK_VERIFY;
    assert(returnValue);

    stackErrorField error = {};

    if (!stk->size) 
    {
        error.anti_overflow = 1;
        return error;
    }
    if (stk->size * REALLOC_RATE  * REALLOC_RATE <= stk->capacity) stackRealloc(stk);

    *returnValue = stk->data[--stk->size];
    //stk->data[stk->size] = 0;

    stk->hash = stackHashCalc(stk);
    return error;
}

stackErrorField stackRealloc(stack *stk)
{
    STACK_VERIFY;

    printf("i'm stackRealloc\n");
    printf("capacity = %lld\nsize = %lld\n", stk->capacity, stk->size);
    printf("start reallocing\n");

    stackErrorField error = {};

    stk->data = (elem_t *)((int *)stk->data - 1);
    size_t prevCapacity = stk->capacity;

    if (stk->size >= stk->capacity)
    {
        stk->capacity *= REALLOC_RATE;
        stk->data = (elem_t *)realloc(stk->data, stk->capacity * sizeof(elem_t) + 2 * sizeof(int));
    }
    else if (stk->size * REALLOC_RATE  * REALLOC_RATE <= stk->capacity)
    {
        stk->capacity /= REALLOC_RATE;
        stk->data = (elem_t *)realloc(stk->data, stk->capacity * sizeof(elem_t) + 2 * sizeof(int));
    }
    if (!stk->data) 
    {
        error.realloc_failed = 1;
        return error;
    }

    stk->data = (int *)stk->data + 1;
    *(unsigned int *)(stk->data + prevCapacity)  = 0;
    *(unsigned int *)(stk->data + stk->capacity) = BUF_CANARY;

    printf("realloc finished\n");
    printf("new capacity = %lld\nnew size = %lld\n", stk->capacity, stk->size);
    return error;
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

unsigned int stackHashCalc(stack *stk)
{
    assert(stk);

    unsigned int tempHash = stk->hash;
    stk->hash = 0;

    unsigned int hash = 0;
    for (size_t i = 0; i < sizeof(stack); i++)
    {
        hash = (hash * HASH_BASE + *((unsigned char *)stk + i)) % HASH_MOD;
    }

    if (!stk->data) return hash;

    size_t bufSize = stk->capacity * sizeof(elem_t);
    for (size_t i = 0; i < bufSize; i++)
    {
        hash = (hash * HASH_BASE + *((unsigned char *)stk->data + i)) % HASH_MOD;
    }

    stk->hash = tempHash;
    return hash;
}

stackErrorField stackHashCheck(stack *stk)
{
    assert(stk);
    stackErrorField error = {};

    unsigned int hashValue = stackHashCalc(stk);
    if (hashValue != stk->hash) error.changed_hash = 1;

    return error;
}
