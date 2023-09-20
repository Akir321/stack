#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main()
{
    stack stk = {};
    stackCtor(&stk, 16);
    printf("%d\n", stackError(&stk));

    stackPush(&stk, 4);
    stackDump(&stk, "test_stack.cpp", 14, "main()");
    //printf("%d %d %d\n", stk.data[0], stk.data[1], stk.data[2]);

    stackDtor(&stk);
    printf("%d\n", stackError(&stk));
}
