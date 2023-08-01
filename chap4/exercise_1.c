/*Write type declarations and constructor functions to express the abstract syntax
of regular expressions.
*/
#include<stdio.h>

// 定义正则表达式抽象语法的结构体
typedef enum 
{
    EMPTY,
    EPSILON,
    LITERAL,
    CONCAT,
    UNION,
    KLEENCLOSURE
} RegexType;

typedef struct Regex 
{
    RegexType type;
    char literal; // 仅当类型为LITERAL时使用
    struct Regex* left; // 用于CONCAT，UNION和KLEENCLOSURE
    struct Regex* right; // 用于CONCAT和UNION
} Regex;

// 构造函数：创建不同类型的正则表达式
Regex* createEmptyRegex() 
{
    Regex* regex=(Regex*)malloc(sizeof(Regex));
    regex->type=EMPTY;
    return regex;
}

Regex* createEpsilonRegex() 
{
    Regex* regex=(Regex*)malloc(sizeof(Regex));
    regex->type=EPSILON;
    return regex;
}

Regex* createLiteralRegex(char character) {
    Regex* regex=(Regex*)malloc(sizeof(Regex));
    regex->type=LITERAL;
    regex->literal=character;
    return regex;
}

Regex* createConcatRegex(Regex* left, Regex* right) 
{
    Regex* regex=(Regex*)malloc(sizeof(Regex));
    regex->type=CONCAT;
    regex->left=left;
    regex->right=right;
    return regex;
}

Regex* createUnionRegex(Regex* left, Regex* right) 
{
    Regex* regex=(Regex*)malloc(sizeof(Regex));
    regex->type=UNION;
    regex->left=left;
    regex->right=right;
    return regex;
}

Regex* createKLEENCLOSURERegex(Regex* regex) 
{
    Regex* KLEENCLOSURE=(Regex*)malloc(sizeof(Regex));
    KLEENCLOSURE->type=KLEENCLOSURE;
    KLEENCLOSURE->left=regex;
    KLEENCLOSURE->right=NULL;
    return KLEENCLOSURE;
}
