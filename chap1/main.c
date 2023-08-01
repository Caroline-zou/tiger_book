#include "slp.h"
#include "prog1.h"
#include "maxargs.h"
#include <stdio.h>

int main()
{
    A_stm stm = prog();
    int ans = maxargs(stm);
    printf("the max number of arguments of all the print statements is: %d\n", ans);
    return 0;
}