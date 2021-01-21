%{
#include <string>
#include "Node.h"
#include "sysy.tab.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    (yylval.token = t)
#define FIX_LINE    for (auto i : std::string(yytext_ptr)) { if (i == '\n') yylineno++; }
%}

%option noyywrap

%%


"if"                                return TOKEN(IF);
"else"                              return TOKEN(ELSE);
"while"                             return TOKEN(WHILE);
"break"                             return TOKEN(BREAK);
"continue"                          return TOKEN(CONTINUE);
"return"                            return TOKEN(RETURN);
"const"                             return TOKEN(CONST);
"int"                               return TOKEN(INT);
"void"                              return TOKEN(VOID);
"putf"[ \t\n]*"("                   FIX_LINE; yylval.string = new std::string("printf"); *yy_cp = yy_hold_char; yy_hold_char='(';yy_cp--; yyleng--; yy_c_buf_p--; return IDENTIFIER;
[a-zA-Z_][a-zA-Z0-9_]*              SAVE_TOKEN; return IDENTIFIER;
[1-9][0-9]*                         SAVE_TOKEN; return INTEGER_VALUE;
"0"[xX][0-9a-fA-F]+                 SAVE_TOKEN; return INTEGER_VALUE;
"0"[0-7]*                           SAVE_TOKEN; return INTEGER_VALUE;

"="                                 return TOKEN(ASSIGN);
"=="                                return TOKEN(EQ);
"!="                                return TOKEN(NE);
"<"                                 return TOKEN(LT);
"<="                                return TOKEN(LE);
">"                                 return TOKEN(GT);
">="                                return TOKEN(GE);
"&&"                                return TOKEN(AND);
"||"                                return TOKEN(OR);

"("                                 return TOKEN(LPAREN);
")"                                 return TOKEN(RPAREN);
"["                                 return TOKEN(LSQUARE);
"]"                                 return TOKEN(RSQUARE);
"{"                                 return TOKEN(LBRACE);
"}"                                 return TOKEN(RBRACE);

","                                 return TOKEN(COMMA);
";"                                 return TOKEN(SEMI);

"+"                                 return TOKEN(PLUS);
"-"                                 return TOKEN(MINUS);
"*"                                 return TOKEN(MUL);
"/"                                 return TOKEN(DIV);
"%"                                 return TOKEN(MOD);
"!"                                 return TOKEN(NOT);

"//"[^\n]*                          ;
"/*"                        		{
                                        int c;
                                        while((c = yyinput()) != 0) {
                                            if(c == '\n')
                                                yylineno++;
                                            else if(c == '*') {
                                                if((c = yyinput()) == '/')
                                                    break;
                                                else
                                                    unput(c);
                                                }
                                        }
                                    }
\n                                  yylineno++;
[ \t\v\f\r]                         ;

.                                   printf("Unknown token: %s\n", yytext); yyterminate();

%%