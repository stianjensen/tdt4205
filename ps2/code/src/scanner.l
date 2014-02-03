%{
/* This %{ ... %} section is code that is glued into the generated scanner. */
#include "parser.h"
#define YYDEBUG 1
extern int outputStage; // This variable is located in vslc.c
    #define RETURN(t) do {                                             \
        if ( outputStage == 1 )                                        \
            fprintf ( stderr, "TOKEN ( %d,\t'%s' )\n", t, yytext );    \
        return t;                                                      \
    } while ( 0 )
    
    
%}

%option array
%option noyywrap
%option yylineno

d [0-9]
l [A-Za-z_]
string \"([^\n\"]|\\\")*\"

%%
[\ \t\n]+           // Ignore whitespace
\/\/[^\n]+          // Ignore comments

TRUE                { RETURN(TRUE_CONST); }
FALSE               { RETURN(FALSE_CONST); }

FUNC                { RETURN(FUNC); }
PRINT               { RETURN(PRINT); }
RETURN              { RETURN(RETURN); }
IF                  { RETURN(IF); }
THEN                { RETURN(THEN); }
ELSE                { RETURN(ELSE); }
END                 { RETURN(END); }
WHILE               { RETURN(WHILE); }
DO                  { RETURN(DO); }
INT                 { RETURN(INT); }
BOOL                { RETURN(BOOL); }
FLOAT               { RETURN(FLOAT); }
CLASS               { RETURN(_CLASS_); }
HAS                 { RETURN(HAS); }
WITH                { RETURN(WITH); }
START               { RETURN(START); }
VOID                { RETURN(VOID); }
THIS                { RETURN(THIS); }
NEW                 { RETURN(NEW); }

":="                { RETURN(ASSIGN); }

"+"                 { RETURN('+'); }
"-"                 { RETURN('-'); }
"*"                 { RETURN('*'); }
"/"                 { RETURN('/'); }

">"|"<"             { RETURN(yytext[0]); }
">="                { RETURN(GEQUAL); }
"<="                { RETURN(LEQUAL); }
"=="                { RETURN(EQUAL); }
"!="                { RETURN(NEQUAL); }

"||"                { RETURN(OR); }
"&&"                { RETURN(AND); }

"!"                 { RETURN('!'); }
"."                 { RETURN('.'); }


{d}+                { RETURN(INT_CONST); }
{d}+\.{d}+          { RETURN(FLOAT_CONST); }

{string}            { RETURN(STRING_CONST); }

{l}({l}|{d})*       { RETURN(IDENTIFIER); }

"("                 { RETURN('('); }
")"                 { RETURN(')'); }
","                 { RETURN(','); }
";"                 { RETURN(';'); }
%%
