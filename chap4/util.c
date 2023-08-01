/*
 * util.c - commonly used utility functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
void *checked_malloc(int len)
{void *p = malloc(len);
 if (!p) {
    fprintf(stderr,"\nRan out of memory!\n");
    exit(1);
 }
 return p;
}

string String(char *s)
{string p = checked_malloc(strlen(s)+1);
 strcpy(p,s);
 return p;
}

// 为一个 string 串最后添加一个 char 字符
string stringAppendChar(string str, char ch)
{   int stringLen = strlen(str);
    string dst = checked_malloc(stringLen+2);
    strcpy(dst, str);
    dst[stringLen] = ch;
    dst[stringLen+1] = '\0';
    free(str);
    return dst;
}



U_boolList U_BoolList(bool head, U_boolList tail)
{ U_boolList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}
