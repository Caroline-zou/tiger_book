/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0
#define YYPATCH 20220114

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#undef YYBTYACC
#define YYBTYACC 0
#define YYDEBUGSTR YYPREFIX "debug"
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "tiger.grm"
#include <stdio.h>
#include "util.h"
#include "errormsg.h"

int yylex(void); /* function prototype */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 15 "tiger.grm"
typedef union YYSTYPE {
	int pos;
	int ival;
	string sval;
	} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 48 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define ID 257
#define STRING 258
#define INT 259
#define COMMA 260
#define COLON 261
#define SEMICOLON 262
#define LPAREN 263
#define RPAREN 264
#define LBRACK 265
#define RBRACK 266
#define LBRACE 267
#define RBRACE 268
#define DOT 269
#define PLUS 270
#define MINUS 271
#define TIMES 272
#define DIVIDE 273
#define EQ 274
#define NEQ 275
#define LT 276
#define LE 277
#define GT 278
#define GE 279
#define AND 280
#define OR 281
#define ASSIGN 282
#define ARRAY 283
#define IF 284
#define THEN 285
#define ELSE 286
#define WHILE 287
#define FOR 288
#define TO 289
#define DO 290
#define LET 291
#define IN 292
#define END 293
#define OF 294
#define BREAK 295
#define NIL 296
#define FUNCTION 297
#define VAR 298
#define TYPE 299
#define UMINUS 300
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    5,
    5,    7,    7,    7,    8,   12,   12,   11,   11,   11,
   13,   13,   14,   14,    9,    9,   10,   10,   15,   15,
   15,    2,    2,    2,    2,    6,    6,   16,   16,    3,
    3,   17,   18,   18,    4,    4,   19,   19,
};
static const YYINT yylen[] = {                            2,
    1,    1,    1,    1,    1,    2,    1,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    4,
    6,    3,    6,    4,    4,    8,    1,    5,    3,    2,
    0,    1,    1,    1,    5,    5,    0,    1,    3,    3,
    4,    0,    5,    0,    4,    6,    8,   10,    8,   10,
    0,    1,    3,    4,    4,    2,    0,    3,    0,    3,
    4,    2,    3,    0,    4,    0,    5,    0,
};
static const YYINT yydefred[] = {                         0,
    0,    5,    4,    0,    0,    0,    0,    0,    0,   27,
    3,    0,    0,    0,    7,    0,    0,    0,    0,    0,
    6,    0,    0,    0,    0,    0,    0,    0,    0,   32,
   33,   34,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   60,    0,    0,
    0,    0,    0,    0,   56,   29,    0,    0,    0,    0,
    0,    0,    0,   30,    0,    0,   10,   11,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   53,    0,    0,
   62,   61,    0,    0,   20,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   54,    0,    0,    0,   58,    0,
    0,    0,    0,    0,    0,   38,    0,    0,    0,   28,
   63,    0,    0,   65,    0,    0,    0,    0,    0,    0,
    0,    0,   35,    0,    0,    0,    0,    0,    0,   39,
   40,    0,    0,    0,    0,   41,    0,    0,    0,    0,
    0,    0,    0,   47,    0,   67,    0,    0,    0,   36,
    0,   48,    0,   43,    0,    0,    0,    0,    0,    0,
    0,   49,    0,   50,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT yystos[] = {                           0,
  257,  258,  259,  263,  271,  284,  287,  288,  291,  295,
  296,  302,  303,  304,  305,  263,  265,  267,  303,  308,
  303,  303,  303,  257,  297,  298,  299,  307,  309,  310,
  311,  312,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  265,  269,  282,  264,  303,  319,
  303,  257,  306,  262,  318,  264,  285,  290,  282,  257,
  257,  257,  292,  307,  303,  303,  303,  303,  303,  303,
  303,  303,  303,  303,  303,  303,  303,  257,  303,  260,
  320,  264,  266,  274,  268,  303,  303,  303,  303,  263,
  261,  282,  274,  308,  266,  303,  294,  303,  318,  286,
  289,  257,  315,  257,  303,  257,  267,  283,  313,  293,
  320,  303,  260,  321,  303,  303,  261,  264,  282,  315,
  294,  299,  314,  257,  290,  257,  261,  274,  303,  268,
  257,  257,  274,  303,  260,  316,  257,  303,  274,  303,
  257,  274,  297,  317,  313,  321,  261,  303,  257,  314,
  257,  317,  263,  316,  315,  264,  261,  274,  257,  303,
  274,  317,  303,  317,
};
#endif /* YYDESTRUCT_CALL || YYSTYPE_TOSTRING */
static const YYINT yydgoto[] = {                         12,
   19,   14,   15,   53,   28,   20,   29,   30,   31,   32,
  109,  123,  103,  136,  144,   55,   50,   81,  114,
};
static const YYINT yysindex[] = {                      -173,
 -185,    0,    0, -173, -173, -173, -173, -244, -259,    0,
    0,    0,  756, -261,    0, -188, -173, -238,  483, -242,
    0,  645,  -28, -224, -197, -183, -178, -220, -259,    0,
    0,    0, -173, -173, -173, -173, -173, -173, -173, -173,
 -173, -173, -173, -173, -173, -170, -173,    0,  -68, -176,
  685, -182, -177, -173,    0,    0, -173, -173, -173, -169,
 -258, -179, -173,    0, -266, -266,    0,    0, -146, -146,
 -146, -146, -146, -146,  240,  200,  725,    0,  756, -173,
    0,    0, -192, -173,    0,  483,  607,  756,  565, -160,
 -153, -173, -255, -181,    0,  -68, -173,  -46,    0, -173,
 -173, -156, -151, -166,  756,    0, -160, -175, -168,    0,
    0,  756, -140,    0,  756,  525, -137, -251, -173, -139,
 -127, -125,    0, -141, -173, -124, -122, -173,  756,    0,
    0, -136, -173,  756, -118,    0, -134,  -99, -255,  -46,
 -120, -173, -114,    0, -168,    0, -113,  -99, -117,    0,
 -124,    0, -160,    0, -119, -243, -110, -173, -126,  -99,
 -173,    0,  -99,    0,
};
static const YYINT yyrindex[] = {                         0,
   41,    0,    0, -115,    0,    0,    0,    0, -142,    0,
    0,    0,  152,   81,    0,    0,    0, -112, -264,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -142,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -111,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -138,    0,  121,  161,    0,    0,  201,  241,
  284,  324,  364,  404,  444,  484,    0,    0,  524,    0,
    0,    0,    1,    0,    0, -264,  726,  566,    0, -107,
    0,    0,    0,    0,    0, -111,    0, -109,    0,    0,
    0,    0,    0,    0, -265,    0, -108,    0, -233,    0,
    0,  606,    0,    0,  646,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -191,    0,    0, -262,    0,
    0,    0,    0,  686,    0,    0,    0, -237,    0, -109,
    0,    0,    0,    0, -233,    0,    0, -237,    0,    0,
 -191,    0, -107,    0,    0,    0,    0,    0,    0, -237,
    0,    0, -237,    0,
};
#if YYBTYACC
static const YYINT yycindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
#endif
static const YYINT yygindex[] = {                         0,
    9,    0,    0,    0,  125,   95,    0,    0,    0,    0,
   23,   18,  -96,   13, -143,   79,    0,   70,   28,
};
#define YYTABLESIZE 1037
static const YYINT yytable[] = {                         59,
   55,  106,   91,   45,  152,   35,   36,   46,   13,  127,
  120,  107,   24,   21,   22,   23,  162,  157,   52,  164,
   47,   56,  128,   92,   49,   51,   45,  108,   59,   46,
  158,   45,   45,   45,   46,   46,   46,   25,   26,   27,
   52,   65,   66,   67,   68,   69,   70,   71,   72,   73,
   74,   75,   76,   77,   51,   79,  155,   59,   37,   60,
   51,   51,   86,   37,   37,   87,   88,   89,    1,    2,
    3,   63,   44,   61,    4,   48,   44,   16,   62,   17,
    2,   18,    5,    1,    2,    3,   78,   82,   96,    4,
   85,   84,   98,   90,   93,    6,  102,    5,    7,    8,
  105,   97,    9,  104,  117,  112,   10,   11,  115,  116,
    6,  110,  118,    7,    8,  119,  124,    9,  121,  126,
    8,   10,   11,   33,   34,   35,   36,  129,  130,  131,
  122,  132,  133,  134,  137,  135,  138,  139,  141,  142,
  147,  140,  149,  151,  156,  153,  159,  161,   57,   31,
  148,    1,   64,   64,   57,   66,   42,   94,   68,   42,
    9,  145,  150,  154,   99,  111,  160,  146,    0,  163,
   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,
   43,   44,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   80,    0,    0,    0,    0,    0,  143,    0,    0,
   12,   33,   34,   35,   36,   37,   38,   39,   40,   41,
   42,   43,   44,  113,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   33,   34,   35,   36,   37,   38,   39,
   40,   41,   42,   43,   44,    0,    0,    0,    0,    0,
   13,   33,   34,   35,   36,   37,   38,   39,   40,   41,
   42,   43,   44,    0,    0,    0,    0,    0,    0,    0,
   55,   58,   55,    0,   55,   55,   55,    0,   55,   55,
   55,   55,   55,   55,   55,   55,   55,   55,   55,   55,
   55,   55,   55,   14,    0,   55,   55,    0,    0,   55,
   55,    0,   55,   55,    0,    0,    0,   55,   55,   55,
   52,    0,   52,    0,   52,    0,   52,    0,   52,   52,
   52,   52,   52,   52,   52,   52,   52,   52,   52,   52,
   52,   52,   52,   15,    0,   52,   52,    0,    0,   52,
   52,    0,   52,   52,    0,    0,    0,   52,   52,   52,
    2,    0,    2,    0,    2,    0,    2,    0,    2,    0,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,    2,    0,   16,    0,    2,    2,    0,    0,    2,
    2,    0,    2,    2,    0,    0,    0,    2,    2,    2,
    8,    0,    8,    0,    8,    0,    8,    0,    8,    0,
    8,    8,    0,    0,    8,    8,    8,    8,    8,    8,
    8,    8,    0,   17,    0,    8,    8,    0,    0,    8,
    8,    0,    8,    8,    0,    0,    0,    8,    8,    8,
    9,    0,    9,    0,    9,    0,    9,    0,    9,    0,
    9,    9,    0,    0,    9,    9,    9,    9,    9,    9,
    9,    9,    0,   18,    0,    9,    9,    0,    0,    9,
    9,    0,    9,    9,    0,    0,    0,    9,    9,    9,
   12,    0,   12,    0,   12,    0,   12,    0,   12,   33,
   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,
   12,   12,    0,   19,    0,   12,   12,    0,    0,   12,
   12,    0,   12,   12,    0,    0,    0,   12,   12,   12,
   13,    0,   13,    0,   13,    0,   13,    0,   13,   33,
   34,   35,   36,   37,   38,   39,   40,   41,   42,    0,
   13,   13,    0,   22,    0,   13,   13,    0,    0,   13,
   13,    0,   13,   13,    0,    0,    0,   13,   13,   13,
    0,    0,    0,   14,    0,   14,    0,   14,    0,   14,
    0,   14,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   14,   14,   25,    0,    0,   14,   14,
    0,    0,   14,   14,    0,   14,   14,    0,    0,    0,
   14,   14,   14,   15,    0,   15,    0,   15,    0,   15,
    0,   15,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   15,   15,   21,    0,    0,   15,   15,
    0,    0,   15,   15,    0,   15,   15,    0,    0,    0,
   15,   15,   15,   16,    0,   16,    0,   16,    0,   16,
    0,   16,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   16,   16,   23,    0,    0,   16,   16,
    0,    0,   16,   16,    0,   16,   16,    0,    0,    0,
   16,   16,   16,   17,    0,   17,    0,   17,    0,   17,
    0,   17,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   17,   17,   26,    0,    0,   17,   17,
    0,    0,   17,   17,    0,   17,   17,    0,    0,    0,
   17,   17,   17,   18,    0,   18,    0,   18,    0,   18,
    0,   18,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   18,   18,   24,    0,    0,   18,   18,
    0,    0,   18,   18,    0,   18,   18,    0,    0,    0,
   18,   18,   18,   19,   54,   19,    0,   19,    0,   19,
    0,   19,   33,   34,   35,   36,   37,   38,   39,   40,
   41,   42,   43,   44,   19,    0,    0,    0,   19,   19,
    0,    0,   19,   19,    0,   19,   19,    0,    0,    0,
   19,   19,   19,   22,    0,   22,    0,   22,    0,   22,
    0,   22,    0,    0,   33,   34,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,    0,    0,   22,   22,
    0,    0,   22,   22,  125,   22,   22,    0,    0,    0,
   22,   22,   22,    0,    0,   25,    0,   25,    0,   25,
    0,   25,    0,   25,   33,   34,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,    0,    0,    0,    0,
   25,   25,    0,  101,   25,   25,    0,   25,   25,    0,
    0,    0,   25,   25,   25,   21,    0,   21,    0,   21,
    0,   21,    0,   21,    0,    0,   33,   34,   35,   36,
   37,   38,   39,   40,   41,   42,   43,   44,    0,    0,
   21,   21,  100,    0,   21,   21,    0,   21,   21,    0,
    0,    0,   21,   21,   21,   23,    0,   23,    0,   23,
    0,   23,    0,   23,   33,   34,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,    0,    0,    0,   57,
   23,   23,    0,    0,   23,   23,    0,   23,   23,    0,
    0,    0,   23,   23,   23,   26,    0,   26,    0,   26,
   83,   26,    0,   26,   33,   34,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,    0,    0,    0,    0,
   26,   26,    0,    0,   26,   26,    0,   26,   26,    0,
    0,    0,   26,   26,   26,   24,    0,   24,    0,   24,
   95,   24,    0,   24,   33,   34,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,    0,    0,    0,    0,
   24,    0,    0,    0,   24,   24,    0,   24,   24,    0,
    0,    0,   24,   24,   24,   33,   34,   35,   36,   37,
   38,   39,   40,   41,   42,   43,   44,
};
static const YYINT yycheck[] = {                        264,
    0,  257,  261,  265,  148,  272,  273,  269,    0,  261,
  107,  267,  257,    5,    6,    7,  160,  261,  257,  163,
  282,  264,  274,  282,   16,   17,  292,  283,  293,  292,
  274,  297,  298,  299,  297,  298,  299,  297,  298,  299,
    0,   33,   34,   35,   36,   37,   38,   39,   40,   41,
   42,   43,   44,   45,  292,   47,  153,  282,  292,  257,
  298,  299,   54,  297,  298,   57,   58,   59,  257,  258,
  259,  292,  264,  257,  263,  264,  268,  263,  257,  265,
    0,  267,  271,  257,  258,  259,  257,  264,   80,  263,
  268,  274,   84,  263,  274,  284,  257,  271,  287,  288,
   92,  294,  291,  257,  261,   97,  295,  296,  100,  101,
  284,  293,  264,  287,  288,  282,  257,  291,  294,  257,
    0,  295,  296,  270,  271,  272,  273,  119,  268,  257,
  299,  257,  274,  125,  257,  260,  128,  274,  257,  274,
  261,  133,  257,  257,  264,  263,  257,  274,  264,  292,
  142,    0,  264,   29,  293,  268,  264,   63,  268,  268,
    0,  139,  145,  151,   86,   96,  158,  140,   -1,  161,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  260,   -1,   -1,   -1,   -1,   -1,  297,   -1,   -1,
    0,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  281,  260,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  279,  280,  281,   -1,   -1,   -1,   -1,   -1,
    0,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  281,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  260,  290,  262,   -1,  264,  265,  266,   -1,  268,  269,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
  260,   -1,  262,   -1,  264,   -1,  266,   -1,  268,  269,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
  260,   -1,  262,   -1,  264,   -1,  266,   -1,  268,   -1,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,   -1,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
  260,   -1,  262,   -1,  264,   -1,  266,   -1,  268,   -1,
  270,  271,   -1,   -1,  274,  275,  276,  277,  278,  279,
  280,  281,   -1,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
  260,   -1,  262,   -1,  264,   -1,  266,   -1,  268,   -1,
  270,  271,   -1,   -1,  274,  275,  276,  277,  278,  279,
  280,  281,   -1,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
  260,   -1,  262,   -1,  264,   -1,  266,   -1,  268,  270,
  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,
  280,  281,   -1,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
  260,   -1,  262,   -1,  264,   -1,  266,   -1,  268,  270,
  271,  272,  273,  274,  275,  276,  277,  278,  279,   -1,
  280,  281,   -1,    0,   -1,  285,  286,   -1,   -1,  289,
  290,   -1,  292,  293,   -1,   -1,   -1,  297,  298,  299,
   -1,   -1,   -1,  260,   -1,  262,   -1,  264,   -1,  266,
   -1,  268,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  280,  281,    0,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,   -1,  292,  293,   -1,   -1,   -1,
  297,  298,  299,  260,   -1,  262,   -1,  264,   -1,  266,
   -1,  268,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  280,  281,    0,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,   -1,  292,  293,   -1,   -1,   -1,
  297,  298,  299,  260,   -1,  262,   -1,  264,   -1,  266,
   -1,  268,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  280,  281,    0,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,   -1,  292,  293,   -1,   -1,   -1,
  297,  298,  299,  260,   -1,  262,   -1,  264,   -1,  266,
   -1,  268,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  280,  281,    0,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,   -1,  292,  293,   -1,   -1,   -1,
  297,  298,  299,  260,   -1,  262,   -1,  264,   -1,  266,
   -1,  268,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  280,  281,    0,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,   -1,  292,  293,   -1,   -1,   -1,
  297,  298,  299,  260,  262,  262,   -1,  264,   -1,  266,
   -1,  268,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  281,   -1,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,   -1,  292,  293,   -1,   -1,   -1,
  297,  298,  299,  260,   -1,  262,   -1,  264,   -1,  266,
   -1,  268,   -1,   -1,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,   -1,   -1,  285,  286,
   -1,   -1,  289,  290,  290,  292,  293,   -1,   -1,   -1,
  297,  298,  299,   -1,   -1,  260,   -1,  262,   -1,  264,
   -1,  266,   -1,  268,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,   -1,   -1,   -1,   -1,
  285,  286,   -1,  289,  289,  290,   -1,  292,  293,   -1,
   -1,   -1,  297,  298,  299,  260,   -1,  262,   -1,  264,
   -1,  266,   -1,  268,   -1,   -1,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,   -1,   -1,
  285,  286,  286,   -1,  289,  290,   -1,  292,  293,   -1,
   -1,   -1,  297,  298,  299,  260,   -1,  262,   -1,  264,
   -1,  266,   -1,  268,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,   -1,   -1,   -1,  285,
  285,  286,   -1,   -1,  289,  290,   -1,  292,  293,   -1,
   -1,   -1,  297,  298,  299,  260,   -1,  262,   -1,  264,
  266,  266,   -1,  268,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,   -1,   -1,   -1,   -1,
  285,  286,   -1,   -1,  289,  290,   -1,  292,  293,   -1,
   -1,   -1,  297,  298,  299,  260,   -1,  262,   -1,  264,
  266,  266,   -1,  268,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,   -1,   -1,   -1,   -1,
  285,   -1,   -1,   -1,  289,  290,   -1,  292,  293,   -1,
   -1,   -1,  297,  298,  299,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,
};
#if YYBTYACC
static const YYINT yyctable[] = {                        -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,
};
#endif
#define YYFINAL 12
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 300
#define YYUNDFTOKEN 322
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"$end",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"error","ID","STRING","INT","COMMA",
"COLON","SEMICOLON","LPAREN","RPAREN","LBRACK","RBRACK","LBRACE","RBRACE","DOT",
"PLUS","MINUS","TIMES","DIVIDE","EQ","NEQ","LT","LE","GT","GE","AND","OR",
"ASSIGN","ARRAY","IF","THEN","ELSE","WHILE","FOR","TO","DO","LET","IN","END",
"OF","BREAK","NIL","FUNCTION","VAR","TYPE","UMINUS","$accept","program","exp",
"lvalue","funcall","recordarg","decs","expseq","dec","tydec","vardec","fundec",
"ty","tydec1","tyfields","tyfields1","fundec1","expseq1","callarg","callarg1",
"recordarg1","illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : program",
"program : exp",
"exp : lvalue",
"exp : NIL",
"exp : INT",
"exp : STRING",
"exp : MINUS exp",
"exp : funcall",
"exp : exp PLUS exp",
"exp : exp MINUS exp",
"exp : exp TIMES exp",
"exp : exp DIVIDE exp",
"exp : exp EQ exp",
"exp : exp NEQ exp",
"exp : exp LT exp",
"exp : exp LE exp",
"exp : exp GT exp",
"exp : exp GE exp",
"exp : exp AND exp",
"exp : exp OR exp",
"exp : ID LBRACE recordarg RBRACE",
"exp : ID LBRACK exp RBRACK OF exp",
"exp : lvalue ASSIGN exp",
"exp : IF exp THEN exp ELSE exp",
"exp : IF exp THEN exp",
"exp : WHILE exp DO exp",
"exp : FOR ID ASSIGN exp TO exp DO exp",
"exp : BREAK",
"exp : LET decs IN expseq END",
"exp : LPAREN expseq RPAREN",
"decs : dec decs",
"decs :",
"dec : tydec",
"dec : vardec",
"dec : fundec",
"tydec : TYPE ID EQ ty tydec1",
"tydec1 : TYPE ID EQ ty tydec1",
"tydec1 :",
"ty : ID",
"ty : LBRACE tyfields RBRACE",
"ty : ARRAY OF ID",
"tyfields : ID COLON ID tyfields1",
"tyfields :",
"tyfields1 : COMMA ID COLON ID tyfields1",
"tyfields1 :",
"vardec : VAR ID ASSIGN exp",
"vardec : VAR ID COLON ID ASSIGN exp",
"fundec : FUNCTION ID LPAREN tyfields RPAREN EQ exp fundec1",
"fundec : FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp fundec1",
"fundec1 : FUNCTION ID LPAREN tyfields RPAREN EQ exp fundec1",
"fundec1 : FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp fundec1",
"fundec1 :",
"lvalue : ID",
"lvalue : lvalue DOT ID",
"lvalue : lvalue LBRACK exp RBRACK",
"lvalue : ID LBRACK exp RBRACK",
"expseq : exp expseq1",
"expseq :",
"expseq1 : SEMICOLON exp expseq1",
"expseq1 :",
"funcall : ID LPAREN RPAREN",
"funcall : ID LPAREN callarg RPAREN",
"callarg : exp callarg1",
"callarg1 : COMMA exp callarg1",
"callarg1 :",
"recordarg : ID EQ exp recordarg1",
"recordarg :",
"recordarg1 : COMMA ID EQ exp recordarg1",
"recordarg1 :",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE  yyloc; /* position returned by actions */
YYLTYPE  yylloc; /* position from the lexer */
#endif

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(loc, rhs, n) \
do \
{ \
    if (n == 0) \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 0).last_line; \
        (loc).first_column = YYRHSLOC(rhs, 0).last_column; \
        (loc).last_line    = YYRHSLOC(rhs, 0).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, 0).last_column; \
    } \
    else \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 1).first_line; \
        (loc).first_column = YYRHSLOC(rhs, 1).first_column; \
        (loc).last_line    = YYRHSLOC(rhs, n).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, n).last_column; \
    } \
} while (0)
#endif /* YYLLOC_DEFAULT */
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#if YYBTYACC

#ifndef YYLVQUEUEGROWTH
#define YYLVQUEUEGROWTH 32
#endif
#endif /* YYBTYACC */

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#ifndef YYINITSTACKSIZE
#define YYINITSTACKSIZE 200
#endif

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  *p_base;
    YYLTYPE  *p_mark;
#endif
} YYSTACKDATA;
#if YYBTYACC

struct YYParseState_s
{
    struct YYParseState_s *save;    /* Previously saved parser state */
    YYSTACKDATA            yystack; /* saved parser stack */
    int                    state;   /* saved parser state */
    int                    errflag; /* saved error recovery status */
    int                    lexeme;  /* saved index of the conflict lexeme in the lexical queue */
    YYINT                  ctry;    /* saved index in yyctable[] for this conflict */
};
typedef struct YYParseState_s YYParseState;
#endif /* YYBTYACC */
/* variables for the parser stack */
static YYSTACKDATA yystack;
#if YYBTYACC

/* Current parser state */
static YYParseState *yyps = 0;

/* yypath != NULL: do the full parse, starting at *yypath parser state. */
static YYParseState *yypath = 0;

/* Base of the lexical value queue */
static YYSTYPE *yylvals = 0;

/* Current position at lexical value queue */
static YYSTYPE *yylvp = 0;

/* End position of lexical value queue */
static YYSTYPE *yylve = 0;

/* The last allocated position at the lexical value queue */
static YYSTYPE *yylvlim = 0;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
/* Base of the lexical position queue */
static YYLTYPE *yylpsns = 0;

/* Current position at lexical position queue */
static YYLTYPE *yylpp = 0;

/* End position of lexical position queue */
static YYLTYPE *yylpe = 0;

/* The last allocated position at the lexical position queue */
static YYLTYPE *yylplim = 0;
#endif

/* Current position at lexical token queue */
static YYINT  *yylexp = 0;

static YYINT  *yylexemes = 0;
#endif /* YYBTYACC */

/* For use in generated program */
#define yydepth (int)(yystack.s_mark - yystack.s_base)
#if YYBTYACC
#define yytrial (yyps->save)
#endif /* YYBTYACC */

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE *newps;
#endif

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    newps = (YYLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
    if (newps == 0)
        return YYENOMEM;

    data->p_base = newps;
    data->p_mark = newps + i;
#endif

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;

#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%sdebug: stack size increased to %d\n", YYPREFIX, newsize);
#endif
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    free(data->p_base);
#endif
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif /* YYPURE || defined(YY_NO_LEAKS) */
#if YYBTYACC

static YYParseState *
yyNewState(unsigned size)
{
    YYParseState *p = (YYParseState *) malloc(sizeof(YYParseState));
    if (p == NULL) return NULL;

    p->yystack.stacksize = size;
    if (size == 0)
    {
        p->yystack.s_base = NULL;
        p->yystack.l_base = NULL;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        p->yystack.p_base = NULL;
#endif
        return p;
    }
    p->yystack.s_base    = (YYINT *) malloc(size * sizeof(YYINT));
    if (p->yystack.s_base == NULL) return NULL;
    p->yystack.l_base    = (YYSTYPE *) malloc(size * sizeof(YYSTYPE));
    if (p->yystack.l_base == NULL) return NULL;
    memset(p->yystack.l_base, 0, size * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    p->yystack.p_base    = (YYLTYPE *) malloc(size * sizeof(YYLTYPE));
    if (p->yystack.p_base == NULL) return NULL;
    memset(p->yystack.p_base, 0, size * sizeof(YYLTYPE));
#endif

    return p;
}

static void
yyFreeState(YYParseState *p)
{
    yyfreestack(&p->yystack);
    free(p);
}
#endif /* YYBTYACC */

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#if YYBTYACC
#define YYVALID        do { if (yyps->save)            goto yyvalid; } while(0)
#define YYVALID_NESTED do { if (yyps->save && \
                                yyps->save->save == 0) goto yyvalid; } while(0)
#endif /* YYBTYACC */

int
YYPARSE_DECL()
{
    int yym, yyn, yystate, yyresult;
#if YYBTYACC
    int yynewerrflag;
    YYParseState *yyerrctx = NULL;
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  yyerror_loc_range[3]; /* position of error start/end (0 unused) */
#endif
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
    if (yydebug)
        fprintf(stderr, "%sdebug[<# of symbols on state stack>]\n", YYPREFIX);
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    memset(yyerror_loc_range, 0, sizeof(yyerror_loc_range));
#endif

#if YYBTYACC
    yyps = yyNewState(0); if (yyps == 0) goto yyenomem;
    yyps->save = 0;
#endif /* YYBTYACC */
    yym = 0;
    /* yyn is set below */
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base;
#endif
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
#if YYBTYACC
        do {
        if (yylvp < yylve)
        {
            /* we're currently re-reading tokens */
            yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc = *yylpp++;
#endif
            yychar = *yylexp++;
            break;
        }
        if (yyps->save)
        {
            /* in trial mode; save scanner results for future parse attempts */
            if (yylvp == yylvlim)
            {   /* Enlarge lexical value queue */
                size_t p = (size_t) (yylvp - yylvals);
                size_t s = (size_t) (yylvlim - yylvals);

                s += YYLVQUEUEGROWTH;
                if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL) goto yyenomem;
                if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL) goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL) goto yyenomem;
#endif
                yylvp   = yylve = yylvals + p;
                yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp   = yylpe = yylpsns + p;
                yylplim = yylpsns + s;
#endif
                yylexp  = yylexemes + p;
            }
            *yylexp = (YYINT) YYLEX;
            *yylvp++ = yylval;
            yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *yylpp++ = yylloc;
            yylpe++;
#endif
            yychar = *yylexp++;
            break;
        }
        /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
        yychar = YYLEX;
#if YYBTYACC
        } while (0);
#endif /* YYBTYACC */
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, " <%s>", YYSTYPE_TOSTRING(yychar, yylval));
#endif
            fputc('\n', stderr);
        }
#endif
    }
#if YYBTYACC

    /* Do we have a conflict? */
    if (((yyn = yycindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
        yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        YYINT ctry;

        if (yypath)
        {
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
                                YYDEBUGSTR, yydepth, yystate);
#endif
            /* Switch to the next conflict context */
            save = yypath;
            yypath = save->save;
            save->save = NULL;
            ctry = save->ctry;
            if (save->state != yystate) YYABORT;
            yyFreeState(save);

        }
        else
        {

            /* Unresolved conflict - start/continue trial parse */
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
            {
                fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", YYDEBUGSTR, yydepth, yystate);
                if (yyps->save)
                    fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
                else
                    fputs("Starting trial parse.\n", stderr);
            }
#endif
            save                  = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (save == NULL) goto yyenomem;
            save->save            = yyps->save;
            save->state           = yystate;
            save->errflag         = yyerrflag;
            save->yystack.s_mark  = save->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (save->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            save->yystack.l_mark  = save->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (save->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            save->yystack.p_mark  = save->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (save->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            ctry                  = yytable[yyn];
            if (yyctable[ctry] == -1)
            {
#if YYDEBUG
                if (yydebug && yychar >= YYEOF)
                    fprintf(stderr, "%s[%d]: backtracking 1 token\n", YYDEBUGSTR, yydepth);
#endif
                ctry++;
            }
            save->ctry = ctry;
            if (yyps->save == NULL)
            {
                /* If this is a first conflict in the stack, start saving lexemes */
                if (!yylexemes)
                {
                    yylexemes = (YYINT *) malloc((YYLVQUEUEGROWTH) * sizeof(YYINT));
                    if (yylexemes == NULL) goto yyenomem;
                    yylvals   = (YYSTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYSTYPE));
                    if (yylvals == NULL) goto yyenomem;
                    yylvlim   = yylvals + YYLVQUEUEGROWTH;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpsns   = (YYLTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYLTYPE));
                    if (yylpsns == NULL) goto yyenomem;
                    yylplim   = yylpsns + YYLVQUEUEGROWTH;
#endif
                }
                if (yylvp == yylve)
                {
                    yylvp  = yylve = yylvals;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp  = yylpe = yylpsns;
#endif
                    yylexp = yylexemes;
                    if (yychar >= YYEOF)
                    {
                        *yylve++ = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                        *yylpe++ = yylloc;
#endif
                        *yylexp  = (YYINT) yychar;
                        yychar   = YYEMPTY;
                    }
                }
            }
            if (yychar >= YYEOF)
            {
                yylvp--;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp--;
#endif
                yylexp--;
                yychar = YYEMPTY;
            }
            save->lexeme = (int) (yylvp - yylvals);
            yyps->save   = save;
        }
        if (yytable[yyn] == ctry)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                                YYDEBUGSTR, yydepth, yystate, yyctable[ctry]);
#endif
            if (yychar < 0)
            {
                yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp++;
#endif
                yylexp++;
            }
            if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                goto yyoverflow;
            yystate = yyctable[ctry];
            *++yystack.s_mark = (YYINT) yystate;
            *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *++yystack.p_mark = yylloc;
#endif
            yychar  = YYEMPTY;
            if (yyerrflag > 0) --yyerrflag;
            goto yyloop;
        }
        else
        {
            yyn = yyctable[ctry];
            goto yyreduce;
        }
    } /* End of code dealing with conflicts */
#endif /* YYBTYACC */
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                            YYDEBUGSTR, yydepth, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yylloc;
#endif
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;
#if YYBTYACC

    yynewerrflag = 1;
    goto yyerrhandler;
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */

yyerrlab:
    /* explicit YYERROR from an action -- pop the rhs of the rule reduced
     * before looking for error recovery */
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif

    yynewerrflag = 0;
yyerrhandler:
    while (yyps->save)
    {
        int ctry;
        YYParseState *save = yyps->save;
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
                            YYDEBUGSTR, yydepth, yystate, yyps->save->state,
                    (int)(yylvp - yylvals - yyps->save->lexeme));
#endif
        /* Memorize most forward-looking error state in case it's really an error. */
        if (yyerrctx == NULL || yyerrctx->lexeme < yylvp - yylvals)
        {
            /* Free old saved error context state */
            if (yyerrctx) yyFreeState(yyerrctx);
            /* Create and fill out new saved error context state */
            yyerrctx                 = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (yyerrctx == NULL) goto yyenomem;
            yyerrctx->save           = yyps->save;
            yyerrctx->state          = yystate;
            yyerrctx->errflag        = yyerrflag;
            yyerrctx->yystack.s_mark = yyerrctx->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (yyerrctx->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yyerrctx->yystack.l_mark = yyerrctx->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (yyerrctx->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yyerrctx->yystack.p_mark = yyerrctx->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (yyerrctx->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yyerrctx->lexeme         = (int) (yylvp - yylvals);
        }
        yylvp          = yylvals   + save->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yylpp          = yylpsns   + save->lexeme;
#endif
        yylexp         = yylexemes + save->lexeme;
        yychar         = YYEMPTY;
        yystack.s_mark = yystack.s_base + (save->yystack.s_mark - save->yystack.s_base);
        memcpy (yystack.s_base, save->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
        yystack.l_mark = yystack.l_base + (save->yystack.l_mark - save->yystack.l_base);
        memcpy (yystack.l_base, save->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yystack.p_mark = yystack.p_base + (save->yystack.p_mark - save->yystack.p_base);
        memcpy (yystack.p_base, save->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
        ctry           = ++save->ctry;
        yystate        = save->state;
        /* We tried shift, try reduce now */
        if ((yyn = yyctable[ctry]) >= 0) goto yyreduce;
        yyps->save     = save->save;
        save->save     = NULL;
        yyFreeState(save);

        /* Nothing left on the stack -- error */
        if (!yyps->save)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
                                YYPREFIX, yydepth);
#endif
            /* Restore state as it was in the most forward-advanced error */
            yylvp          = yylvals   + yyerrctx->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylpp          = yylpsns   + yyerrctx->lexeme;
#endif
            yylexp         = yylexemes + yyerrctx->lexeme;
            yychar         = yylexp[-1];
            yylval         = yylvp[-1];
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc         = yylpp[-1];
#endif
            yystack.s_mark = yystack.s_base + (yyerrctx->yystack.s_mark - yyerrctx->yystack.s_base);
            memcpy (yystack.s_base, yyerrctx->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yystack.l_mark = yystack.l_base + (yyerrctx->yystack.l_mark - yyerrctx->yystack.l_base);
            memcpy (yystack.l_base, yyerrctx->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yystack.p_mark = yystack.p_base + (yyerrctx->yystack.p_mark - yyerrctx->yystack.p_base);
            memcpy (yystack.p_base, yyerrctx->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yystate        = yyerrctx->state;
            yyFreeState(yyerrctx);
            yyerrctx       = NULL;
        }
        yynewerrflag = 1;
    }
    if (yynewerrflag == 0) goto yyinrecovery;
#endif /* YYBTYACC */

    YYERROR_CALL("syntax error");
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yyerror_loc_range[1] = yylloc; /* lookahead position is error start position */
#endif

#if !YYBTYACC
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
#endif
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* lookahead position is error end position */
                yyerror_loc_range[2] = yylloc;
                YYLLOC_DEFAULT(yyloc, yyerror_loc_range, 2); /* position of error span */
                *++yystack.p_mark = yyloc;
#endif
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* the current TOS position is the error start position */
                yyerror_loc_range[1] = *yystack.p_mark;
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
                if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark, yystack.p_mark);
#else
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
                --yystack.s_mark;
                --yystack.l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                --yystack.p_mark;
#endif
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
        }
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval, &yylloc);
#else
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
    yym = yylen[yyn];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
                        YYDEBUGSTR, yydepth, yystate, yyn, yyrule[yyn]);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            if (yym > 0)
            {
                int i;
                fputc('<', stderr);
                for (i = yym; i > 0; i--)
                {
                    if (i != yym) fputs(", ", stderr);
                    fputs(YYSTYPE_TOSTRING(yystos[yystack.s_mark[1-i]],
                                           yystack.l_mark[1-i]), stderr);
                }
                fputc('>', stderr);
            }
#endif
        fputc('\n', stderr);
    }
#endif
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

    /* Perform position reduction */
    memset(&yyloc, 0, sizeof(yyloc));
#if YYBTYACC
    if (!yytrial)
#endif /* YYBTYACC */
    {
        YYLLOC_DEFAULT(yyloc, &yystack.p_mark[-yym], yym);
        /* just in case YYERROR is invoked within the action, save
           the start of the rhs as the error start position */
        yyerror_loc_range[1] = yystack.p_mark[1-yym];
    }
#endif

    switch (yyn)
    {
    default:
        break;
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
        {
            fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[YYFINAL], yyval));
#endif
            fprintf(stderr, "shifting from state 0 to final state %d\n", YYFINAL);
        }
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yyloc;
#endif
        if (yychar < 0)
        {
#if YYBTYACC
            do {
            if (yylvp < yylve)
            {
                /* we're currently re-reading tokens */
                yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylloc = *yylpp++;
#endif
                yychar = *yylexp++;
                break;
            }
            if (yyps->save)
            {
                /* in trial mode; save scanner results for future parse attempts */
                if (yylvp == yylvlim)
                {   /* Enlarge lexical value queue */
                    size_t p = (size_t) (yylvp - yylvals);
                    size_t s = (size_t) (yylvlim - yylvals);

                    s += YYLVQUEUEGROWTH;
                    if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL)
                        goto yyenomem;
                    if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL)
                        goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL)
                        goto yyenomem;
#endif
                    yylvp   = yylve = yylvals + p;
                    yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp   = yylpe = yylpsns + p;
                    yylplim = yylpsns + s;
#endif
                    yylexp  = yylexemes + p;
                }
                *yylexp = (YYINT) YYLEX;
                *yylvp++ = yylval;
                yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                *yylpp++ = yylloc;
                yylpe++;
#endif
                yychar = *yylexp++;
                break;
            }
            /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
            yychar = YYLEX;
#if YYBTYACC
            } while (0);
#endif /* YYBTYACC */
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
                                YYDEBUGSTR, yydepth, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[yystate], yyval));
#endif
        fprintf(stderr, "shifting from state %d to state %d\n", *yystack.s_mark, yystate);
    }
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    *++yystack.p_mark = yyloc;
#endif
    goto yyloop;
#if YYBTYACC

    /* Reduction declares that this path is valid. Set yypath and do a full parse */
yyvalid:
    if (yypath) YYABORT;
    while (yyps->save)
    {
        YYParseState *save = yyps->save;
        yyps->save = save->save;
        save->save = yypath;
        yypath = save;
    }
#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
                        YYDEBUGSTR, yydepth, yystate, yypath->state, (int)(yylvp - yylvals - yypath->lexeme));
#endif
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    yylvp          = yylvals + yypath->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yylpp          = yylpsns + yypath->lexeme;
#endif
    yylexp         = yylexemes + yypath->lexeme;
    yychar         = YYEMPTY;
    yystack.s_mark = yystack.s_base + (yypath->yystack.s_mark - yypath->yystack.s_base);
    memcpy (yystack.s_base, yypath->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
    yystack.l_mark = yystack.l_base + (yypath->yystack.l_mark - yypath->yystack.l_base);
    memcpy (yystack.l_base, yypath->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base + (yypath->yystack.p_mark - yypath->yystack.p_base);
    memcpy (yystack.p_base, yypath->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
    yystate        = yypath->state;
    goto yyloop;
#endif /* YYBTYACC */

yyoverflow:
    YYERROR_CALL("yacc stack overflow");
#if YYBTYACC
    goto yyabort_nomem;
yyenomem:
    YYERROR_CALL("memory exhausted");
yyabort_nomem:
#endif /* YYBTYACC */
    yyresult = 2;
    goto yyreturn;

yyabort:
    yyresult = 1;
    goto yyreturn;

yyaccept:
#if YYBTYACC
    if (yyps->save) goto yyvalid;
#endif /* YYBTYACC */
    yyresult = 0;

yyreturn:
#if defined(YYDESTRUCT_CALL)
    if (yychar != YYEOF && yychar != YYEMPTY)
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval, &yylloc);
#else
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */

    {
        YYSTYPE *pv;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYLTYPE *pp;

        for (pv = yystack.l_base, pp = yystack.p_base; pv <= yystack.l_mark; ++pv, ++pp)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv, pp);
#else
        for (pv = yystack.l_base; pv <= yystack.l_mark; ++pv)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
    }
#endif /* defined(YYDESTRUCT_CALL) */

#if YYBTYACC
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    while (yyps)
    {
        YYParseState *save = yyps;
        yyps = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
    while (yypath)
    {
        YYParseState *save = yypath;
        yypath = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
#endif /* YYBTYACC */
    yyfreestack(&yystack);
    return (yyresult);
}
