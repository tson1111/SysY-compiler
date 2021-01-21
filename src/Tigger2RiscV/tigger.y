%{
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
extern int yydebug;

extern int yylex();
extern int yyget_lineno();

extern std::ostream *out;
int stk;
extern std::map<char, std::string> token2str;
void yyerror(const char *s) { std::printf("Error(line: %d): %s\n", yyget_lineno(), s); if (!yydebug) std::exit(1); }
#define YYERROR_VERBOSE true
#define YYDEBUG 1
%}

%union {
    int token;
    std::string *string;
}
%token <token> INTEGER
%token <string> OP
%token <string> LABEL VARIABLE FUNCTION REGISTER
%token <token> IF RETURN END GOTO CALL PARAM
%token <token> LOAD STORE LOADADDR MALLOC
%token <token> ASSIGN COLON LSQUARE RSQUARE EOL

%start CompUnit

%%

CompUnit: CompUnit GlobalDecl 
        | CompUnit FuncDef 
        | GlobalDecl
        | FuncDef
        | CompUnit EOL
        | {}
        ;

GlobalDecl: VARIABLE ASSIGN INTEGER EOL  {
        *out << "\t.global " << *$1 << "\n";
        *out << "\t.section .sdata\n";
        *out << "\t.align 2\n";
        *out << "\t.type " << *$1 << ", @object\n";
        *out << "\t.size " << *$1 << ", 4\n";
        *out << *$1 << ":\n";
        *out << "\t.word " << $3 << "\n";
    }
    | VARIABLE ASSIGN MALLOC INTEGER EOL {
        *out << "\t.comm " << *$1 << ", " << $4 * 4 << ", 4\n";
    } 
    ;

FuncDef: FuncBegin FuncDefStmt FuncEnd
    ;

FuncBegin: FUNCTION LSQUARE INTEGER RSQUARE LSQUARE INTEGER RSQUARE EOL {
        stk = ($6 / 4 + 1) * 16;
        std::string func_name = $1->substr(2);
        *out << "\t.text\n";
        *out << "\t.align 2\n";
        *out << "\t.global " << func_name << "\n";
        *out << "\t.type " << func_name << ", @function\n";
        *out << func_name << ":\n";
        *out << "\tadd sp, sp, " << -stk << "\n";
        *out << "\tsw ra, " << stk - 4 << "(sp)\n";
    }
    | FuncBegin EOL
    ;

FuncDefStmt: FuncDefStmt AssignmentStmt
    | FuncDefStmt ConditionStmt
    | FuncDefStmt GotoStmt
    | FuncDefStmt LabelStmt
    | FuncDefStmt FuncCallStmt
    | FuncDefStmt LoadStoreStmt
    | FuncDefStmt ReturnStmt
    | FuncDefStmt EOL
    | {}
    ;

AssignmentStmt: REGISTER ASSIGN REGISTER OP REGISTER EOL {
        switch($4->at(0)) {
            case '&':
            *out << "\tsnez s0, " << *$3 << "\n";
            *out << "\tsnez " << *$1 << ", " << *$5 << "\n";
            *out << "\tand " << *$1 << ", " << *$1 << ", s0\n";
            break;
            case '|':
            *out << "\tor " << *$1 << ", " << *$3 << ", " << *$5 << "\n";
            *out << "\tsnez " << *$1 << ", " << *$1 << "\n";
            break;
            case '=':
            *out << "\txor " << *$1 << ", " << *$3 << ", " << *$5 << "\n";
            *out << "\tseqz " << *$1 << ", " << *$1 << "\n";
            break;
            case '!':
            *out << "\txor " << *$1 << ", " << *$3 << ", " << *$5 << "\n";
            *out << "\tsnez " << *$1 << ", " << *$1 << "\n";
            break;
            default:
            *out << "\t" << token2str[$4->at(0)] << " " << *$1 << ", " << *$3 << ", " << *$5 << "\n";
            break;
        }
        
    }
    | REGISTER ASSIGN REGISTER OP INTEGER EOL {
        *out << "\t" << token2str[$4->at(0)] << "i " << *$1 << ", " << *$3 << ", " << $5 << "\n";
    }
    | REGISTER ASSIGN OP REGISTER EOL {
        if ($3->at(0) == '!') {
            *out << "\txori " << *$1 << ", " << *$4 << ", -1\n"; 
            *out << "\tsnez " << *$1 << ", " << *$1 << "\n";
        }
        else {
            *out << "\t" << token2str[$3->at(0)] << " " << *$1 << ", x0, " << *$4 << "\n";
        }
    }
    | REGISTER ASSIGN REGISTER EOL {
        *out << "\taddi " << *$1 << ", " << *$3 << ", 0\n";
    }
    | REGISTER ASSIGN INTEGER EOL {
        *out << "\tli " << *$1 << ", " << $3 << "\n";
    }
    | REGISTER LSQUARE INTEGER RSQUARE ASSIGN REGISTER EOL {
        *out << "\tsw " << *$6 << ", " << $3 << "(" << *$1 << ")\n";
    }
    | REGISTER ASSIGN REGISTER LSQUARE INTEGER RSQUARE EOL {
        *out << "\tlw " << *$1 << ", " << $5 << "(" << *$3 << ")\n";
    }
    ;

ConditionStmt: IF REGISTER OP REGISTER GOTO LABEL EOL {
        if (*$3 == "<") {
            *out << "\tblt " << *$2 << ", " << *$4 << ", ." << *$6 << "\n";
        }
        else if (*$3 == ">") {
            *out << "\tblt " << *$4 << ", " << *$2 << ", ." << *$6 << "\n";
        }
        else if (*$3 == "!=") {
            *out << "\tbne " << *$2 << ", " << *$4 << ", ." << *$6 << "\n";
        }
        else if (*$3 == "==") {
            *out << "\tbeq " << *$2 << ", " << *$4 << ", ." << *$6 << "\n";
        }
        else if (*$3 == "<=") {
            *out << "\tble " << *$2 << ", " << *$4 << ", ." << *$6 << "\n";
        }
        else if (*$3 == ">=") {
            *out << "\tble " << *$4 << ", " << *$2 << ", ." << *$6 << "\n";
        }
        
    }
    ;

GotoStmt: GOTO LABEL EOL {
        *out << "\tj ." << *$2 << "\n";
    }
    ;

LabelStmt: LABEL COLON EOL {
        *out << "." << *$1 << ":\n";
    }
    ;

FuncCallStmt: CALL FUNCTION EOL {
        std::string func_name = $2->substr(2);
        *out << "\tcall " << func_name  << "\n";
    }
    ;

LoadStoreStmt: LOAD INTEGER REGISTER EOL {
        *out << "\tlw " << *$3 << ", " << $2 * 4 << "(sp)\n";
    }
    | LOAD VARIABLE REGISTER EOL {
        *out << "\tlui " << *$3 << ", %hi(" << *$2 << ")\n";
        *out << "\tlw " << *$3 << ", %lo(" << *$2 << ")(" << *$3 << ")\n";
    }
    | LOADADDR INTEGER REGISTER EOL {
        *out << "\taddi " << *$3 << ", sp, " << $2 * 4 << "\n";
    }
    | LOADADDR VARIABLE REGISTER EOL {
        *out << "\tlui " << *$3 << ", %hi(" << *$2 << ")\n";
        *out << "\tadd " << *$3 << ", " << *$3 << ", %lo(" << *$2 << ")\n";
    }
    | STORE REGISTER INTEGER EOL {
        *out << "\tsw " << *$2 << ", " << $3 * 4 << "(sp)\n";
    }
    ;

ReturnStmt: RETURN EOL {
        *out << "\tlw ra, " << stk - 4 << "(sp)\n" ;
        *out << "\taddi sp, sp, " << stk << "\n";
        *out << "\tjr ra\n";
    }
    ;

FuncEnd: END FUNCTION EOL {
        std::string func_name = $2->substr(2);
        *out << "\t.size " << func_name << ", .-" << func_name << "\n";
    }
    ;
