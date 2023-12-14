#include <stdio.h>
#include <assert.h>

#include "stack.h"
#include "html_logfile.h"


int main()
{
    stack stk = {};
    LOG("sizeof(stack) = %lld\n", sizeof(stack));
    for (size_t i = 0; i < sizeof(stack); i++)
    {
        printf("%d", *((char *)&stk + i));
    }
    putchar('\n');
    STACK_DUMP(&stk);
    stackCtor(&stk, 2);
    //printf("hash calc  = %u\n", stackHashCalc(&stk));
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
    LOG("popped = %d\n", a);
    STACK_DUMP(&stk);
    stackPop(&stk, &a);
    stackPop(&stk, &a);
    stackPop(&stk, &a);
    STACK_DUMP(&stk);
    stackPop(&stk, &a);
    STACK_DUMP(&stk);

    stackPush(&stk, 1);
    STACK_DUMP(&stk);

    LOG("hi\n");

    /*for (char *ptr = (char *)&stk; *ptr !=  '\0'; ptr++)
    {
        printf("%c\n", *ptr);
        *ptr = 42;
        printf("stk.data = %p\n", stk.data);
    }
    printf("stk.data = %p\n", stk.data);
    stackPush(&stk, 78);*/

    stackDtor(&stk);
    LOG("hi after destruction\n");
    LOG("error (after destructing) = %d\n", errorFieldToU(stackError(&stk)));
    stackPush(&stk, 2);
}
