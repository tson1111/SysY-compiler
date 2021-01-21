#pragma once
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Symtab.h"
#include "Tigger.h"

class Node {
public:
    virtual ~Node();
    virtual void generate_Tigger(Symtab& symt, TiggerList& el) = 0;
};

class NRoot: public Node {
public:
    std::vector<Node*> body;
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NExpression: public Node {
public:
    virtual void generate_Tigger(Symtab& symt, TiggerList& el) = 0;
};

class NStatement: public NExpression{
public:
  virtual void generate_Tigger(Symtab& symt, TiggerList& el) = 0;
};

class NRightValue: public NExpression {}; 


class NVariable : public NRightValue {
public:
  std::string name;
  NVariable(const std::string &name_);
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NFunction : public NExpression {
public:
  std::string name;
  NFunction(const std::string &name_);
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NLabel : public NExpression {
public:
  std::string name;
  NLabel(const std::string &name_);
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NNumber : public NRightValue {
 public:
  std::int32_t value;
  NNumber(const std::string& value_);
  NNumber(std::int32_t value_);
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NVarDeclare : public NStatement {
 public:
  NVariable& name;
  NVarDeclare(NVariable& name_);                
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NArrayDeclare : public NStatement {
 public:
  NVariable& name;
  int size;
  NArrayDeclare(NVariable& name_, NNumber& num);            
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NFuncDefStatement : public NStatement {
public:
    std::vector<Node*> body;
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NFunctionDefine : public NStatement {
public:
  NFunction &name;
  int param_num;
  NFuncDefStatement &stmt;
  NFunctionDefine(NFunction &name_, NNumber &num_, NFuncDefStatement& stmt_);
  virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NFunctionCall: public NExpression {
public:
    NFunction& name;
    NFunctionCall(NFunction& name_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NAssignment: public NStatement {
  public:
    NExpression& lhs;
    NExpression& rhs;
    NAssignment(NExpression& lhs_, NExpression& rhs_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};


class NArrayExpression: public NExpression {
  public:
    NVariable& name;
    NRightValue& idx;
    NArrayExpression(NVariable& n, NRightValue& i);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};

class NBinaryExpression: public NExpression {
  public:
    NRightValue& lhs;
    NRightValue& rhs;
    int op;
    NBinaryExpression(NRightValue& lhs_, int op_, NRightValue& rhs_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};

class NUnaryExpression: public NExpression {
  public:
    int op;
    NRightValue& rhs;
    NUnaryExpression(int op_, NRightValue& rhs_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};



class NGotoStatement: public NStatement {
  public:
    NLabel& l;
    NGotoStatement(NLabel& l_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NLogicExpression: public NExpression {
  public:
    NRightValue& lhs;
    NRightValue& rhs;
    int op;
    NLogicExpression(NRightValue& lhs_, int op_, NRightValue& rhs_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NConditionStatement: public NStatement {
  public:
    NLogicExpression& cond;
    NGotoStatement& l;
    NConditionStatement(NLogicExpression& cond_, NGotoStatement& l_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};

class NLabelStatement: public NStatement {
  public:
    NLabel l;
    NLabelStatement(NLabel& l_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};

class NParamStatement: public NStatement {
  public:
    NRightValue& rl;
    NParamStatement(NRightValue& rl_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};

class NReturnStatement: public NStatement {
  public:
    NRightValue& rl;
    NReturnStatement(NRightValue& rl_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};

class NReturnVoidStatement: public NStatement {
  public:
    NReturnVoidStatement();
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);

};

class NFuncCallAssignment: public NStatement {
  public:
    NFunction& func;
    NVariable& var;
    NFuncCallAssignment(NVariable& var_, NFunction& func_);
    virtual void generate_Tigger(Symtab& symt, TiggerList& el);
};