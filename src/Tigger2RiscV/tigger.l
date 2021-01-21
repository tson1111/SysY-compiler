
%{
#include <string>
#include "tigger.tab.hpp"

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
variable   v{integer}
function   f_{identifier}
register   [atsx]{integer}

%%


"if"                                return TOKEN(IF);
"return"                            return TOKEN(RETURN);
"end"                               return TOKEN(END);                             
"goto"                              return TOKEN(GOTO);
"call"                              return TOKEN(CALL); 
"param"                             return TOKEN(PARAM);
"load"                              return TOKEN(LOAD);
"store"                             return TOKEN(STORE);
"loadaddr"                          return TOKEN(LOADADDR);
"malloc"                            return TOKEN(MALLOC);



{integer}                           yylval.token = std::stoi(yytext); return INTEGER;

{label}                             SAVE_TOKEN; return LABEL;
{variable}                          SAVE_TOKEN; return VARIABLE;
{function}                          SAVE_TOKEN; return FUNCTION;
{register}                          SAVE_TOKEN; return REGISTER;


"="                                 return TOKEN(ASSIGN);
"=="                                SAVE_TOKEN; return OP;
"!="                                SAVE_TOKEN; return OP;
"<"                                 SAVE_TOKEN; return OP;
">"                                 SAVE_TOKEN; return OP;
"&&"                                SAVE_TOKEN; return OP;
"||"                                SAVE_TOKEN; return OP;
"+"                                 SAVE_TOKEN; return OP;
"-"                                 SAVE_TOKEN; return OP;
"*"                                 SAVE_TOKEN; return OP;
"/"                                 SAVE_TOKEN; return OP;
"%"                                 SAVE_TOKEN; return OP;
"!"                                 SAVE_TOKEN; return OP;

"["                                 return TOKEN(LSQUARE);
"]"                                 return TOKEN(RSQUARE);

":"                                 return TOKEN(COLON);

"//"[^\n]*                          ;
\n                                  yylineno++; return TOKEN(EOL);
[ \t\v\f\r]                         ;

.                                   printf("Unknown token: %s\n", yytext); yyterminate();

%%