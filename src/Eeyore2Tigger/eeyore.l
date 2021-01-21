%{
#include <string>
#include "Node.h"
#include "eeyore.tab.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    (yylval.token = t)
#define FIX_LINE    for (auto i : std::string(yytext_ptr)) { if (i == '\n') yylineno++; }
%}

%option noyywrap

digit      [0-9]
integer    (-)?{digit}+

letter     [A-Za-z_]
identifier {letter}({letter}|{digit})*

label      l{integer}
variable   [Ttp]{integer}
function   f_{identifier}

%%


"if"                                return TOKEN(IF);
"return"                            return TOKEN(RETURN);
"end"                               return TOKEN(END);                                 
"var"                               return TOKEN(VAR); 
"goto"                              return TOKEN(GOTO); 
"call"                              return TOKEN(CALL); 
"param"                             return TOKEN(PARAM); 

{integer}                           SAVE_TOKEN; return INTEGER;

{label}                             SAVE_TOKEN; return LABEL;
{variable}                          SAVE_TOKEN; return VARIABLE;
{function}                          SAVE_TOKEN; return FUNCTION;


"="                                 return TOKEN(ASSIGN);
"=="                                return TOKEN(EQ);
"!="                                return TOKEN(NE);
"<"                                 return TOKEN(LT);
">"                                 return TOKEN(GT);
"&&"                                return TOKEN(AND);
"||"                                return TOKEN(OR);

"["                                 return TOKEN(LSQUARE);
"]"                                 return TOKEN(RSQUARE);

":"                                 return TOKEN(COLON);

"+"                                 return TOKEN(PLUS);
"-"                                 return TOKEN(MINUS);
"*"                                 return TOKEN(MUL);
"/"                                 return TOKEN(DIV);
"%"                                 return TOKEN(MOD);
"!"                                 return TOKEN(NOT);

"//"[^\n]*                          ;
\n                                  yylineno++;
[ \t\v\f\r]                         ;

.                                   printf("Unknown token: %s\n", yytext); yyterminate();

%%