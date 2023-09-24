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

    #ifdef CANARY_PROTECTION
    if (stk->stackCanary1 != STK_CANARY)       error.changed_canary = 1;
    if (stk->stackCanary2 != STK_CANARY)       error.changed_canary = 1;
    #endif

    #ifdef HASH_PROTECTION
    if (stackHashCheck(stk).changed_hash == 1) error.changed_hash   = 1;
    #endif

    if (error.data_null == 1)                  return error;

    #ifdef CANARY_PROTECTION
    unsigned int buf_canary = *((unsigned int *)stk->data - 1);
    if (buf_canary != BUF_CANARY)              error.changed_canary = 1;
    buf_canary = *(unsigned int *)(stk->data + stk->capacity);
    if (buf_canary != BUF_CANARY)              error.changed_canary = 1;
    #endif

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

    #ifdef CANARY_PROTECTION
    stk->stackCanary1 = STK_CANARY;
    stk->stackCanary2 = STK_CANARY;
    #endif

    stk->data = (elem_t *)myCalloc(stk->capacity, sizeof(elem_t));
    //printf("data[%p], capacity = %lld\n", stk->data, stk->capacity);
    if (!stk->data) 
    {
        error.data_null = 1;
        return error;
    }

    #ifdef CANARY_PROTECTION
    *(unsigned int *) stk->data                  = BUF_CANARY;
    stk->data = (elem_t *)((int *)stk->data + 1);
    *(unsigned int *)(stk->data + stk->capacity) = BUF_CANARY;
    #endif

    #ifdef HASH_PROTECTION
    stk->hash = stackHashCalc(stk);
    printf("stackHashCalc = %u\n", stackHashCalc(stk));
    printf("stkHash = %u\n", stk->hash);
    #endif

    return error;
}

stackErrorField stackDtor(stack *stk)
{
    assert(stk);
    stackErrorField error = {};
    
    #ifdef CANARY_PROTECTION
    stk->data = (elem_t *)((int *)stk->data - 1);
    #endif
    
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
    stackErrorField error = stackError(stk);
    printStackErrors(error);

    printf(" capacity = %lld\n size = %lld\n", stk->capacity, stk->size);

    #ifdef HASH_PROTECTION
    printf(" hash = %u\n", stk->hash);
    #endif

    #ifdef CANARY_PROTECTION
    printf("stackCanary1  = 0x%x\n", stk->stackCanary1);
    printf("stackCanary2  = 0x%x\n", stk->stackCanary2);
    #endif

    printf(" data[%p] = ", stk->data);
    if (!stk->data)
    {
        printf("NULL\n");
        return error;
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

    #ifdef CANARY_PROTECTION
    unsigned int buf_canary = *((unsigned int *)stk->data - 1);
    printf("bufferCanary1 = 0x%x\n", buf_canary);
    buf_canary = *(unsigned int *)(stk->data + stk->capacity);
    printf("bufferCanary2 = 0x%x\n", buf_canary);
    #endif

    return error;
}

void printStackErrors(stackErrorField error)
{
    if (error.stack_null)       printf("stack_null     = 1\n");
    if (error.data_null)        printf("data_null      = 1\n");
    if (error.small_capacity)   printf("small_capacity = 1\n");
    if (error.anti_overflow)    printf("anti_overflow  = 1\n");
    if (error.realloc_failed)   printf("realloc_failed = 1\n");

    #ifdef CANARY_PROTECTION
    if (error.changed_canary)   printf("changed_canary = 1\n");
    #endif

    #ifdef HASH_PROTECTION
    if (error.changed_hash)     printf("changed_hash   = 1\n");
    #endif
}

stackErrorField stackPush(stack *stk, elem_t value)
{
    STACK_VERIFY;
    stackErrorField error = {};

    if (stk->size >= stk->capacity) error = stackRealloc(stk);
    if (error.realloc_failed) return error;

    stk->data[stk->size++] = value;

    #ifdef HASH_PROTECTION
    stk->hash = stackHashCalc(stk);
    #endif

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
    if (stk->size * REALLOC_RATE * REALLOC_RATE <= stk->capacity) error =  stackRealloc(stk);
    if (error.realloc_failed) return error;

    *returnValue = stk->data[--stk->size];
    //stk->data[stk->size] = 0;

    #ifdef HASH_PROTECTION
    stk->hash = stackHashCalc(stk);
    #endif

    return error;
}

stackErrorField stackRealloc(stack *stk)
{
    STACK_VERIFY;

    printf("i'm stackRealloc\n");
    printf("capacity = %lld\nsize = %lld\n", stk->capacity, stk->size);
    printf("start reallocing\n");

    stackErrorField error = {};

    #ifdef CANARY_PROTECTION
    stk->data = (elem_t *)((int *)stk->data - 1);
    size_t prevCapacity = stk->capacity;
    #endif

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

    #ifdef CANARY_PROTECTION
    stk->data = (int *)stk->data + 1;
    *(unsigned int *)(stk->data + prevCapacity)  = 0;
    *(unsigned int *)(stk->data + stk->capacity) = BUF_CANARY;
    #endif

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

#ifdef HASH_PROTECTION
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
#endif
