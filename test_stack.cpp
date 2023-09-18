#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main()
{
    printf("Hello, world\n");
    
    stack stk = {};
    stackCtor(&stk, 16);
    printf("%d\n", stackError(&stk));

    stackDtor(&stk);
    printf("%d\n", stackError(&stk));
}