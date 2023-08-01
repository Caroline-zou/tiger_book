%{
#include <string.h>
#include "util.h"
#include "y.tab.h"
#include "errormsg.h"

int charPos=1; // 记录单词位置，值为相对文件开始的字符数
int commentCount=0; // 用于嵌套注释的计数和消减

int yywrap(void)
{
 charPos=1;
 return 1;
}

// 更新当前单词的位置
void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}

%state CONST_STRING OUT_STRING
/* 声明状态 */
%s BLOCK_COMMENT LINE_COMMENT

%%
<INITIAL>" "	{adjust(); continue;} /* 空白处理 */
<INITIAL>\n	    {adjust(); EM_newline(); continue;}
<INITIAL>\r
<INITIAL>\t	    {adjust(); continue;}

<INITIAL>","	{adjust(); return COMMA;} /* 符号 */
<INITIAL>":"  {adjust(); return COLON;}
<INITIAL>";"  {adjust(); return SEMICOLON;}
<INITIAL>"("  {adjust(); return LPAREN;}
<INITIAL>")"  {adjust(); return RPAREN;}
<INITIAL>"["  {adjust(); return LBRACK;}
<INITIAL>"]"  {adjust(); return RBRACK;}
<INITIAL>"{"  {adjust(); return LBRACE;}
<INITIAL>"}"  {adjust(); return RBRACE;}
<INITIAL>"."  {adjust(); return DOT;}
<INITIAL>"+"  {adjust(); return PLUS;}
<INITIAL>"-"  {adjust(); return MINUS;}
<INITIAL>"*"  {adjust(); return TIMES;}
<INITIAL>"/"  {adjust(); return DIVIDE;}
<INITIAL>"="  {adjust(); return EQ;}
<INITIAL>"<>" {adjust(); return NEQ;}
<INITIAL>"<"  {adjust(); return LT;}
<INITIAL>"<=" {adjust(); return LE;}
<INITIAL>">"  {adjust(); return GT;}
<INITIAL>">=" {adjust(); return GE;}
<INITIAL>"&"  {adjust(); return AND;}
<INITIAL>"|"  {adjust(); return OR;}
<INITIAL>":=" {adjust(); return ASSIGN;}

<INITIAL>array   {adjust(); return ARRAY;} /* 关键字 */
<INITIAL>if      {adjust(); return IF;}
<INITIAL>then    {adjust(); return THEN;}
<INITIAL>else    {adjust(); return ELSE;}
<INITIAL>while   {adjust(); return WHILE;}
<INITIAL>for     {adjust(); return FOR;}
<INITIAL>to      {adjust(); return TO;}
<INITIAL>do      {adjust(); return DO;}
<INITIAL>let     {adjust(); return LET;}
<INITIAL>in      {adjust(); return IN;}
<INITIAL>end     {adjust(); return END;}
<INITIAL>of      {adjust(); return OF;}
<INITIAL>break   {adjust(); return BREAK;}
<INITIAL>nil     {adjust(); return NIL;}
<INITIAL>function   {adjust(); return FUNCTION;}
<INITIAL>var     {adjust(); return VAR;}
<INITIAL>type    {adjust(); return TYPE;}

<INITIAL>[0-9]*	 {adjust(); yylval.ival=atoi(yytext); return INT;} /* 标识符 */
<INITIAL>[_a-zA-Z][_a-zA-z0-9]* {adjust(); yylval.sval=String(yytext); return ID;}

<INITIAL>"/*"   {adjust(); commentCount++; BEGIN BLOCK_COMMENT;} /* 块注释起始 */
<BLOCK_COMMENT>{
"/*"        {adjust(); commentCount++;} /* 被嵌套注释起始 */
\n          {adjust(); EM_newline();}
.           {adjust();}
"*/"        {adjust(); commentCount--; if(commentCount == 0) BEGIN INITIAL;}
<<EOF>>     {adjust(); EM_error(EM_tokPos,"unmatched close comment"); return 0;}
}

<INITIAL>"//"   {adjust(); BEGIN LINE_COMMENT;} /* 行注释起始 */
<LINE_COMMENT>{
.           {adjust();}
\n          {adjust(); BEGIN INITIAL;}
<<EOF>>     {adjust(); return 0;}
}

<INITIAL>\"      {adjust(); yylval.sval=String(""); BEGIN CONST_STRING;} /* 字符串字面量 */
<CONST_STRING>{
\"          {adjust(); BEGIN INITIAL; return STRING;} /* 配对双引号，字符串匹配结束并返回 */
\\n         {adjust(); yylval.sval=stringAppendChar(yylval.sval, '\n');} /* 换行符 \n */
\\t         {adjust(); yylval.sval=stringAppendChar(yylval.sval, '\t');} /* 制表符 \t */
\\[a-zA-Z]  {adjust(); yylval.sval=stringAppendChar(yylval.sval, yytext[1]);} /* 控制字符c */
\\[0-9]{3}  {adjust(); yylval.sval=stringAppendChar(yylval.sval, atoi(&yytext[1]));} /* \ddd 具有ASCII码ddd（3个十进制数字）的单个字符 */
\\\"        {adjust(); yylval.sval=stringAppendChar(yylval.sval, '"');} /* 双引号 (") */
\\\\        {adjust(); yylval.sval=stringAppendChar(yylval.sval, '\\');} /* 反斜线 (\) */
\\          {adjust(); BEGIN OUT_STRING;} /* 续行符 */
.           {adjust(); yylval.sval=stringAppendChar(yylval.sval, yytext[0]);}
<<EOF>>     {adjust(); EM_error(EM_tokPos,"unclosed string"); return 0;}
}
<OUT_STRING>{
\\              {adjust(); BEGIN CONST_STRING;} /* 结束续航，回到字符串中 */
[ \t\v\f]       {adjust();}
\n              {adjust(); EM_newline();}
"//".*\n        {adjust();} /* 支持在续航后添加行注释 */
<<EOF>>     {adjust(); EM_error(EM_tokPos,"EOF in \\f__f\\"); return 0;}
}

<INITIAL>.	{adjust(); EM_error(EM_tokPos, "illegal token");}
<<EOF>>     {return 0;}

%%