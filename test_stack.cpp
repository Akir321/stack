#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main()
{
    stack stk = {};
    stackCtor(&stk, 2);
    printf("error = %d\n", stackError(&stk));

    stackPush(&stk, 4);
    stackPush(&stk, 6);
    STACK_DUMP(&stk);
    stackPush(&stk, 3);
    stackPush(&stk, 4);
    stackPush(&stk, 6);
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

    stackDtor(&stk);
    printf("error (after destructing) = %d\n", stackError(&stk));
    stackPush(&stk, 2);
}
