#include "interp.h"
#include "util.h"
#include "slp.h"

#include <stddef.h>
#include <stdio.h>

void interp(A_stm stm)
{
    table t = Table(0, 0, NULL);
    interpStm(stm, t);
}
// 构造 table
table Table(string id, int value, table tail)
{
    table t  = (table)checked_malloc(sizeof(*t));
    t -> id = id;
    t -> value = value;
    t -> tail = tail;
    return t;
}

// 构造 int 和 table 对
intAndTable IntAndTable(int i, table t)
{
    intAndTable iat = (intAndTable)checked_malloc(sizeof(*iat));
    iat->i = i;
    iat->t = t;
    return iat;
}

// 查询表
int lookup(table t, string k)
{
    table tmpTable = t;
    while (tmpTable)
    {
        if (tmpTable->id == k) { // 字符char 相同
            return tmpTable->value;
        } else { // 字符不匹配，移向下一节点
            tmpTable = tmpTable->tail;
        }
    }
    return -1;
}

// 更新表
table update(table t, string k, int v)
{
    return Table(k, v, t);
}

// 解释语句 Stm
table interpStm(A_stm stm, table t)
{
    if (stm == NULL) {
        return NULL;
    }
    switch (stm->kind)
    {
    case A_compoundStm: // 复合语句
         t = interpStm(stm->u.compound.stm1, t);
         t = interpStm(stm->u.compound.stm2, t);
         return t;
    case A_assignStm: // 赋值语句
        {
        intAndTable itable = interpExp(stm->u.assign.exp, t); // 先算出表达式的值
        t = update(itable->t, stm->u.assign.id, itable->i); // 更新表中id对应的值
        return t;
        }
    case A_printStm: // 打印语句
        {
        A_expList tmpExpList = stm->u.print.exps;
        while (tmpExpList->kind != A_lastExpList)
        { // 当不是结尾表达式时，打印除最后的表达式列表(ExpList->exp, lastExpList)
            intAndTable iat = interpExp(tmpExpList->u.pair.head, t);
            t = iat->t;
            printf("%i ", iat->i);
            tmpExpList = tmpExpList->u.pair.tail;
        }
        // 当为结尾时，打印最后表达式的值(ExpList->Exp)
        intAndTable iat = interpExp(tmpExpList->u.pair.head, t);
        t = iat->t;
        printf("%i ", iat->i);
        printf("\n");
        return t;
        }
    }
}

// 解释表达式 Exp
intAndTable interpExp(A_exp e, table t)
{
    if (e == NULL)
    {
        return NULL;
    }
    intAndTable iat = IntAndTable(0, t);
    switch (e->kind)
    {
    case A_idExp: // 标识符，查找对应值
        iat->i = lookup(t, e->u.id);
        iat->t = t;
        return iat;
    case A_numExp: // 数字，直接返回值
        iat->i = e->u.num;
        iat->t = t;
        return iat;
    case A_opExp: // 算式，计算结果
        {// 先算左右两子表达式
        int leftResult = interpExp(e->u.op.left, iat->t) ->i;
        int rightResult = interpExp(e->u.op.right, iat->t) ->i;
        // 再合并左右表达式的值
        switch (e->u.op.oper)
        {
        case A_plus:
            iat->i = leftResult + rightResult;
            break;
        case A_minus:
            iat->i = leftResult - rightResult;
            break;
        case A_times:
            iat->i = leftResult * rightResult;
            break;
        case A_div:
            iat->i = leftResult / rightResult;
            break;
        }
        return iat;
        }
    case A_eseqExp:
        iat->t = interpStm(e->u.eseq.stm, iat->t);
        iat = interpExp(e->u.eseq.exp, t);
        return iat;
    }
}

// 解释表达式列表 ExpList
intAndTable interpExpList(A_expList expList, table t)
{
    if (expList == NULL)
    {
        return NULL;
    }
    intAndTable iat = IntAndTable(0, t);
    switch (expList->kind)
    {
    case A_pairExpList:
        iat = interpExp(expList->u.pair.head, iat->t);
        iat = interpExpList(expList->u.pair.tail, iat->t); // 递归处理
        return iat;
    case A_lastExpList:
        iat = interpExp(expList->u.last, iat->t);
        return iat;
    }
}