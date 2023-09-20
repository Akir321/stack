#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main()
{
    printf("Hello, world\n");
    
    stack stk = {};
    stackCtor(&stk, 16);
    printf("%d\n", stackError(&stk));

    stackPush(&stk, 4);
    printf("%d %d %d\n", stk.data[0], stk.data[1], stk.data[2]);

    stackDtor(&stk);
    printf("%d\n", stackError(&stk));
}
