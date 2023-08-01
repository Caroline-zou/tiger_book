#include "maxargs.h"
#include "slp.h"
#include "util.h"
#include <math.h>
#include <stddef.h>
#include <limits.h>
#include <stdio.h>
/*
Write a function `int maxargs(A_stm)` that tells the maximum number
of arguments of any print statement within any subexpression of a given
statement. For example, maxargs(prog) is 2.
Remember that print statements can contain expressions that
contain other print statements.
*/
int max(int a,int b)
{
    return (a > b)? a : b;
}
int maxargs(A_stm stm)
{
    return dfs(stm);
}

int dfs(A_stm stm)
{
    if(stm == NULL)
        return 0;
    if(stm->kind == A_compoundStm)
    {
        return max(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
    }
    else if(stm->kind == A_assignStm)
    {
        return maxargsExp(stm->u.assign.exp);
    }
    else if(stm->kind == A_printStm)
    {
        return max(countArgsExpList(stm->u.print.exps), maxargsExpList(stm->u.print.exps));
    }
}

int countArgsExpList(A_expList expList)
{
    if(expList == NULL)
        return 0;
    if(expList->kind == A_pairExpList)
        return 1+countArgsExpList(expList->u.pair.tail);
    else if(expList->kind == A_lastExpList)
        return 1;
    return 0;
}

int maxargsExp(A_exp exp)
{
    if(exp == NULL)
        return 0;
    if(exp->kind == A_opExp)
        return max(maxargsExp(exp->u.op.left), maxargsExp(exp->u.op.right));
    else if(exp->kind == A_eseqExp)
    {
        return max(dfs(exp->u.eseq.stm), maxargsExp(exp->u.eseq.exp));
    }
    return 0;
}

int maxargsExpList(A_expList expList)
{
    if(expList == NULL)
        return 0;
    if(expList->kind == A_pairExpList)
        return max(maxargsExp(expList->u.pair.head), maxargsExpList(expList->u.pair.tail));
    else if(expList->kind == A_lastExpList)
        return maxargsExp(expList->u.last);
    return 0;
}