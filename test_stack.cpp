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
    stackDump(&stk, "test_stack.cpp", 13, "main()");
    stackPush(&stk, 3);
    stackPush(&stk, 4);
    stackPush(&stk, 6);
    stackPush(&stk, 3);
    stackDump(&stk, "test_stack.cpp", 18, "main()");
    //printf("%d %d %d\n", stk.data[0], stk.data[1], stk.data[2]);

    elem_t a = 0;
    stackPop(&stk, &a);
    printf("popped = %d\n", a);
    stackDump(&stk, "test_stack.cpp", 24, "main()");
    stackPop(&stk, &a);
    stackPop(&stk, &a);
    stackPop(&stk, &a);
    stackDump(&stk, "test_stack.cpp", 28, "main()");
    stackPop(&stk, &a);
    stackDump(&stk, "test_stack.cpp", 30, "main()");

    stackDtor(&stk);
    printf("error (after destructing) = %d\n", stackError(&stk));
}
