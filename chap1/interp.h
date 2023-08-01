#ifndef INTERP_H_
#define INTERP_H_

#include "util.h"
#include "slp.h"

// 键值对 链表
typedef struct table_ *table ;
struct table_
{
    string id;
    int value;
    table tail;
};

// 查询对
typedef struct intAndTable_ *intAndTable;
struct intAndTable_
{
    int i;
    table t;
};

// 主功能函数
void interp(A_stm);
// 解释函数
table interpStm(A_stm s, table t); // 解析语句
intAndTable interpExp(A_exp e, table t); //解析表达式
intAndTable interpExpList(A_expList e, table t); // 解析多表达式

// 接口
table Table(string id, int value, table tail); // 构造链表 Table
intAndTable IntAndTable(int i, table t); // 构造

int lookup(table t, string k); // 查询表
table update(table t, string k, int v); // 更新表

#endif