%{
#include "Node.h"
#include <cstdio>
#include <cstdlib>
NRoot *root; // the top level root node of our final AST
extern int yydebug;

extern int yylex();
extern int yyget_lineno();
void yyerror(const char *s) { std::printf("Error(line: %d): %s\n", yyget_lineno(), s); if (!yydebug) std::exit(1); }
#define YYERROR_VERBOSE true
#define YYDEBUG 1
%}

%union {
    int token;
    NVariable* ident;
    NExpression* expr;
    NRoot* root;
    NVariable* var;
    NFunction* func;
    NLabel* label;
    NNumber* num;
    NFuncDefStatement *funcdefstmt;
    NFunctionCall *funccall;
    NAssignment *assignstmt;
    NConditionStatement *cond;
    NGotoStatement *go2;
    NLabelStatement *labelstmt;
    NParamStatement *param;
    NReturnStatement *ret;
    NReturnVoidStatement *ret_void;
    NFunctionDefine *funcdef;
    NRightValue *rv;
    NBinaryExpression *bexp;
    NLogicExpression *lexp;
    NUnaryExpression *uexp;
    NArrayExpression *aexp;
    NFuncCallAssignment *funccallassign;

//    NDeclareStatement* declare_statement;
//    NFunctionDefine* fundef;
//    NDeclare* declare;
//    NArrayDeclareInitValue* array_init_value;
//    NArrayIdentifier* array_identifier;
//    NFunctionCallArgList* arg_list;
//    NFunctionDefineArgList* fundefarglist;
//    NFunctionDefineArg* fundefarg;
//    NBlock* block;
    NStatement* stmt;
//    NAssignment* assignmentstmt;
//    NIfElseStatement* ifelsestmt;
//    NConditionExpression* condexp;
    std::string *string;
}

%token <string> INTEGER
%token <string> LABEL VARIABLE FUNCTION
%token <token> IF RETURN END VAR GOTO CALL PARAM
%token <token> ASSIGN EQ NE LT GT AND OR
%token <token> LSQUARE RSQUARE
%token <token> COLON
%token <token> PLUS MINUS MUL DIV MOD NOT


//%type <token> AddOp MulOp RelOp UnaryOp BType
//%type <ident> ident LVal
%type <expr>  Expr    
%type <bexp> BinaryExp
%type <lexp> LogicExp
%type <uexp> UnaryExp
%type <aexp> ArrayExpr
%type <var> Variable
%type <func> Function
%type <label> Label
%type <root> CompUnit
%type <num> Number
%type <funcdefstmt> FuncDefStmt
%type <funccall> FuncCallStmt
%type <assignstmt> AssignmentStmt
%type <cond> ConditionStmt
%type <go2> GotoStmt
%type <labelstmt> LabelStmt
%type <param> ParamStmt
%type <ret> ReturnStmt
%type <ret_void> ReturnVoidStmt
%type <stmt> Decl 
%type <funcdef> FuncDef
%type <rv> RightValue
%type <funccallassign> FuncCallAssignmentStmt
//%type <declare_statement> Decl ConstDecl VarDecl ConstDeclStmt VarDeclStmt
//%type <declare> Def DefOne DefArray ConstDef ConstDefOne ConstDefArray
//%type <array_identifier> DefArrayName ArrayItem
//%type <fundef> FuncDef
//%type <fundefarglist> FuncFParams
//%type <fundefarg> FuncFParam FuncFParamArray FuncFParamOne
//%type <array_init_value> InitValArray InitValArrayInner
//%type <arg_list> FuncRParams
//%type <block> Block BlockItems
//%type <condexp> Cond

%start CompUnit

%%

CompUnit: CompUnit Decl { $$->body.push_back($<stmt>2); }
        | CompUnit FuncDef { $$->body.push_back($<funcdef>2); }
        | Decl { root = new NRoot(); $$ = root; $$->body.push_back($<stmt>1); }
        | FuncDef { root = new NRoot(); $$ = root; $$->body.push_back($<funcdef>1); }
        ;

Decl: VAR Variable {$$ = new NVarDeclare(*$2);}
      | VAR Number Variable {$$ = new NArrayDeclare(*$3, *$2);} 
      ;

FuncDef: Function LSQUARE Number RSQUARE FuncDefStmt END Function {$$ = new NFunctionDefine(*$1, *$3, *$5);}
      ;

FuncDefStmt: FuncDefStmt Expr {$$->body.push_back($<expr>2);}
      | FuncDefStmt Decl {$$->body.push_back($<stmt>2);}
      | Decl {$$ = new NFuncDefStatement(); $$->body.push_back($<stmt>1);}
      | Expr {$$ = new NFuncDefStatement(); $$->body.push_back($<expr>1);}
      ;

RightValue: Variable {$$ = $1;}
      | Number {$$ = $1;}
      ;

Expr: AssignmentStmt {$$ = $1;}
      | FuncCallAssignmentStmt {$$ = $1;}
      | ConditionStmt {$$ = $1;}
      | GotoStmt {$$ = $1;}
      | LabelStmt {$$ = $1;}
      | ParamStmt {$$ = $1;}
      | ReturnStmt {$$ = $1;}
      | ReturnVoidStmt {$$ = $1;}
      | FuncCallStmt {$$ = $1;}
      ;

FuncCallStmt: CALL Function {$$ = new NFunctionCall(*$2); }
      ;

AssignmentStmt: Variable ASSIGN BinaryExp {$$ = new NAssignment(*$1, *$3); }
      | Variable ASSIGN UnaryExp {$$ = new NAssignment(*$1, *$3); }
      | Variable ASSIGN RightValue {$$ = new NAssignment(*$1, *$3); }
      | ArrayExpr ASSIGN RightValue {$$ = new NAssignment(*$1, *$3); }
      | Variable ASSIGN ArrayExpr {$$ = new NAssignment(*$1, *$3); }
      ;

FuncCallAssignmentStmt: Variable ASSIGN CALL Function {$$ = new NFuncCallAssignment(*$1, *$4); }
      ;

ArrayExpr:  Variable LSQUARE RightValue RSQUARE {$$ = new NArrayExpression(*$1, *$3); }
      ;

BinaryExp: RightValue PLUS RightValue {$$ = new NBinaryExpression(*$1, $2, *$3); }
      | RightValue MINUS RightValue  {$$ = new NBinaryExpression(*$1, $2, *$3); }
      | RightValue MUL RightValue  {$$ = new NBinaryExpression(*$1, $2, *$3); }
      | RightValue DIV RightValue  {$$ = new NBinaryExpression(*$1, $2, *$3); }
      | RightValue MOD RightValue  {$$ = new NBinaryExpression(*$1, $2, *$3); }
      | RightValue EQ RightValue {$$ = new NBinaryExpression(*$1, $2, *$3);}
      | RightValue NE RightValue {$$ = new NBinaryExpression(*$1, $2, *$3);}
      | RightValue LT RightValue {$$ = new NBinaryExpression(*$1, $2, *$3);}
      | RightValue GT RightValue {$$ = new NBinaryExpression(*$1, $2, *$3);}
      | RightValue AND RightValue {$$ = new NBinaryExpression(*$1, $2, *$3);}
      | RightValue OR RightValue {$$ = new NBinaryExpression(*$1, $2, *$3);}
      ;

UnaryExp: MINUS RightValue  {$$ = new NUnaryExpression($1, *$2); }
      | NOT RightValue {$$ = new NUnaryExpression($1, *$2); }
      ;

ConditionStmt: IF LogicExp GotoStmt {$$ = new NConditionStatement(*$2, *$3); }
      ;

LogicExp: RightValue EQ RightValue {$$ = new NLogicExpression(*$1, $2, *$3);}
      | RightValue NE RightValue {$$ = new NLogicExpression(*$1, $2, *$3);}
      | RightValue LT RightValue {$$ = new NLogicExpression(*$1, $2, *$3);}
      | RightValue GT RightValue {$$ = new NLogicExpression(*$1, $2, *$3);}
      | RightValue AND RightValue {$$ = new NLogicExpression(*$1, $2, *$3);}
      | RightValue OR RightValue {$$ = new NLogicExpression(*$1, $2, *$3);}
      ;

GotoStmt: GOTO Label {$$ = new NGotoStatement(*$2);}
      ;

LabelStmt: Label COLON {$$ = new NLabelStatement(*$1);}
      ;

ParamStmt: PARAM RightValue {$$ = new NParamStatement(*$2);}
      ;

ReturnStmt: RETURN RightValue {$$ = new NReturnStatement(*$2); }
      ;

ReturnVoidStmt: RETURN {$$ = new NReturnVoidStatement();}
      ;



Variable: VARIABLE { $$ = new NVariable(*$1); }
      ;

Number: INTEGER {$$ = new NNumber(*$1); }
Function: FUNCTION {$$ = new NFunction(*$1); }
Label: LABEL {$$ = new NLabel(*$1);}