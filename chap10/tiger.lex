%{
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "y.tab.h"
#include "errormsg.h"
#define STRINGMAX 1024

int charPos=1;

// After referencing the chap3/lex.yy.c which supplied by author, I have 
// found another elegant method to reslove the comment and string.

char string_builder[STRINGMAX + 1];
int string_index = 0;

int comLevel = 0; // record the nested depth about comment

static void append(char c) {
  if(string_index < STRINGMAX) 
      string_builder[string_index++] = c;
  else {
		EM_error(EM_tokPos,"too long string!");
    string_index = 0;
  }
}

static void get_string() {
  string_builder[string_index] = '\0';
  yylval.sval = String(string_builder);
  string_index = 0;
}

int yywrap(void)
{
 if(comLevel) 
   EM_error(EM_tokPos,"unclosed comment!");
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}



%}

id       [A-Za-z]([A-Za-z]|[0-9]|_)* 

%x COM
%x STR

%%
" "|"\t" {adjust(); continue;}
\n	     {adjust(); EM_newline(); continue;}
","	     {adjust(); return COMMA;}
:	       {adjust(); return COLON;}
:=	     {adjust(); return ASSIGN;}
;    	   {adjust(); return SEMICOLON;}
"("      {adjust(); return LPAREN;}
")"      {adjust(); return RPAREN;}
"["      {adjust(); return LBRACK;}
"]"      {adjust(); return RBRACK;}
"{"      {adjust(); return LBRACE;}
"}"      {adjust(); return RBRACE;}
"."      {adjust(); return DOT;}
"+"      {adjust(); return PLUS;}
"-"   	 {adjust(); return MINUS;}
"*"	     {adjust(); return TIMES;}
"/"      {adjust(); return DIVIDE;}
=	       {adjust(); return EQ;}
"<>"	     {adjust(); return NEQ;}
"<="	   {adjust(); return LE;}
"<"	     {adjust(); return LT;}
">="	   {adjust(); return GE;}
">"	     {adjust(); return GT;}
&        {adjust(); return AND;}
"|"      {adjust(); return OR;}
array    {adjust(); return ARRAY;}
if       {adjust(); return IF;}
then     {adjust(); return THEN;}
else     {adjust(); return ELSE;}
while    {adjust(); return WHILE;}
for  	   {adjust(); return FOR;}
to	     {adjust(); return TO;}
do       {adjust(); return DO;}
let 	   {adjust(); return LET;}
in	     {adjust(); return IN;}
end 	   {adjust(); return END;}
of       {adjust(); return OF;}
break    {adjust(); return BREAK;}
nil      {adjust(); return NIL;}
function {adjust(); return FUNCTION;}
var      {adjust(); return VAR;}
type 	   {adjust(); return TYPE;}
[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
{id}	   {adjust(); yylval.sval = String(yytext); return ID;}
\"	     {adjust(); BEGIN(STR); continue;}
<STR>\\n {adjust(); append('\n'); continue;}	
<STR>\\t {adjust(); append('\t'); continue;}	
<STR>\\\\ {adjust(); append('\\'); continue;}	
<STR>\\\" {adjust(); append('"'); continue;}
<STR>\\[0-9]{3} {
  int x;

  adjust();
  x = yytext[1] * 100 + yytext[2] * 10 + yytext[3] - ('0' * 111);
  if(x > 255) 
    EM_error(EM_tokPos, "illegal ascii escape");
  else
    append(x);
  continue;
}
<STR>\\"^"([A-Z]|"["|"]"|"\\"|"^"|_) {
  adjust();
  append(yytext[2] - '@');
  continue;
}
<STR>\\. {
  adjust();
  EM_error(EM_tokPos, "illegal string escape");
  continue;
}
<STR>\n {
	adjust();
	EM_error(EM_tokPos,"unclosed string!");
  EM_newline();
	BEGIN(INITIAL);
  get_string();
  return STRING;
}
<STR>\" {
	adjust();
	BEGIN(INITIAL);
	get_string();
	return STRING;
}
<STR>. {adjust(); append(yytext[0]); continue;}	
"/*" {adjust(); BEGIN(COM); comLevel = 1; continue;}
<COM>"/*" {adjust(); ++comLevel; continue;}
<COM>"*/" {
  adjust();
  --comLevel;
  if(!comLevel)
    BEGIN(INITIAL);
  continue;
}
<COM>\n {adjust(); EM_newline(); continue;}
<COM>. {adjust(); continue;}
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
