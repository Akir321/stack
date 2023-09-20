#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main()
{
    stack stk = {};
    stackCtor(&stk, 16);
    printf("error = %d\n", stackError(&stk));

    stackPush(&stk, 4);
    stackPush(&stk, 6);
    stackPush(&stk, 3);
    stackDump(&stk, "test_stack.cpp", 14, "main()");
    //printf("%d %d %d\n", stk.data[0], stk.data[1], stk.data[2]);

    elem_t a = 0;
    stackPop(&stk, &a);
    printf("popped = %d\n", a);
    stackDump(&stk, "test_stack.cpp", 20, "main()");

    stackDtor(&stk);
    printf("error (after destructing) = %d\n", stackError(&stk));
}
