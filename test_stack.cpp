#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main()
{
    stack stk = {};
    printf("sizeof(stack) = %lld\n", sizeof(stack));
    for (size_t i = 0; i < sizeof(stack); i++)
    {
        printf("%d", *((char *)&stk + i));
    }
    putchar('\n');
    STACK_DUMP(&stk);
    stackCtor(&stk, 2);
    printf("hash calc  = %u\n", stackHashCalc(&stk));
    printf("hash check = %d\n", stackHashCheck(&stk));
    stackError(&stk);
    for (size_t i = 0; i < sizeof(stack); i++)
    {
        printf("%d ", *((char *)&stk + i));
    }
    STACK_DUMP(&stk);

    stackPush(&stk, 4);
    stackPush(&stk, 6);
    STACK_DUMP(&stk);
    stackPush(&stk, 3);
    STACK_DUMP(&stk);
    stackPush(&stk, 4);
    STACK_DUMP(&stk);
    stackPush(&stk, 6);
    STACK_DUMP(&stk);
    stackPush(&stk, 3);
    STACK_DUMP(&stk);
    //printf("%d %d %d\n", stk.data[0], stk.data[1], stk.data[2]);

    elem_t a = 0;
    stackPop(&stk, &a);
    printf("popped = %d\n", a);
    STACK_DUMP(&stk);
    stackPop(&stk, &a);
    stackPop(&stk, &a);
    stackPop(&stk, &a);
    STACK_DUMP(&stk);
    stackPop(&stk, &a);
    STACK_DUMP(&stk);

    for (char *ptr = (char *)&stk; *ptr !=  '\0'; ptr++)
    {
        printf("%c\n", *ptr);
        *ptr = 42;
    }
    stackPush(&stk, 78);

    stackDtor(&stk);
    printf("error (after destructing) = %d\n", stackError(&stk));
    stackPush(&stk, 2);
}
