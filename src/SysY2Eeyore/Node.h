#pragma once
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Symtab.h"
#include "Eeyore.h"

using INTEGER = std::int32_t;

class Node {
public:
    virtual ~Node();
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NRoot: public Node {
public:
    std::vector<Node*> body;
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NExpression: public Node {
public:
    virtual int eval(Symtab& symt);
};

class NStatement: public NExpression{};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string &name_);
    virtual int eval(Symtab& symt);
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NDeclare: public Node {};

class NConditionExpression: public NExpression {
public:
    NExpression &value;
    NConditionExpression(NExpression &value);
    virtual int eval(Symtab& symt);
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NBinaryExpression: public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryExpression(NExpression& lhs, int op, NExpression& rhs);
    virtual int eval(Symtab& symt);
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NUnaryExpression : public NExpression {
 public:
  int op;
  NExpression& rhs;
  NUnaryExpression(int op, NExpression& rhs);
  virtual int eval(Symtab& symt);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NCommaExpression : public NExpression {
 public:
  std::vector<NExpression*> values;
  NCommaExpression();
  virtual int eval(Symtab& symt);
};

class NFunctionCallArgList: public NExpression {
public:
    std::vector<NExpression*> args;
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NFunctionCall: public NExpression {
public:
    NIdentifier& name;
    NFunctionCallArgList& args;
    NFunctionCall(NIdentifier& name, NFunctionCallArgList& args);
    virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NNumber : public NExpression {
 public:
  INTEGER value;
  NNumber(const std::string& value_);
  NNumber(INTEGER value_);
  virtual int eval(Symtab& symt);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NBlock : public NStatement {
 public:
  std::vector<NStatement*> statements;
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NAssignment : public NStatement {
 public:
  NIdentifier& lhs;
  NExpression& rhs;
  NAssignment(NIdentifier& lhs, NExpression& rhs);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NIfElseStatement : public NStatement {
 public:
  NConditionExpression& cond;
  NStatement& thenstmt;
  NStatement& elsestmt;
  NIfElseStatement(NConditionExpression& cond, NStatement& thenstmt,
                   NStatement& elsestmt);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NIfStatement : public NStatement {
 public:
  NConditionExpression& cond;
  NStatement& thenstmt;
  NIfStatement(NConditionExpression& cond, NStatement& thenstmt);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NWhileStatement : public NStatement {
 public:
  NConditionExpression& cond;
  NStatement& dostmt;
  NWhileStatement(NConditionExpression& cond, NStatement& dostmt);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NBreakStatement : public NStatement {
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NContinueStatement : public NStatement {        
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NReturnStatement : public NStatement {
 public:
  NExpression* value;
  NReturnStatement(NExpression* value = NULL);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NEvalStatement : public NStatement {
 public:
  NExpression& value;
  NEvalStatement(NExpression& value);               
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
  virtual int eval(Symtab& symt);
};

class NVoidStatement : public NStatement {
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NDeclareStatement : public NStatement {
 public:
  std::vector<NDeclare*> list;
  int type;
  NDeclareStatement(int type);                 
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NVarDeclareWithInit : public NDeclare {
 public:
  NIdentifier& name;
  NExpression& value;
  bool is_const;
  NVarDeclareWithInit(NIdentifier& name, NExpression& value,
                      bool is_const = false);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NVarDeclare : public NDeclare {
 public:
  NIdentifier& name;
  NVarDeclare(NIdentifier& name);                
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NArrayIdentifier : public NIdentifier {
 public:
  NIdentifier& name;
  std::vector<NExpression*> shape;
  NArrayIdentifier(NIdentifier& name);                     
  virtual int eval(Symtab& symt);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NArrayDeclareInitValue : public NExpression {
 public:
  bool is_number;
  NExpression* value;
  std::vector<NArrayDeclareInitValue*> value_list;
  NArrayDeclareInitValue(bool is_number, NExpression* value);                 
};

class NArrayDeclareWithInit : public NDeclare {
 public:
  NArrayIdentifier& name;
  NArrayDeclareInitValue& value;
  bool is_const;
  NArrayDeclareWithInit(NArrayIdentifier& name, NArrayDeclareInitValue& value,
                        bool is_const = false);                     
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};




class NArrayDeclare : public NDeclare {
 public:
  NArrayIdentifier& name;
  NArrayDeclare(NArrayIdentifier& name);            
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NFunctionDefineArg : public NExpression {
 public:
  int type;
  NIdentifier& name;
  NFunctionDefineArg(int type, NIdentifier& name);
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);       
};

class NFunctionDefineArgList : public NExpression {
 public:
  std::vector<NFunctionDefineArg*> list;
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};

class NFunctionDefine : public Node {
 public:
  int return_type; //268 for int, 269 for void
  NIdentifier& name;
  NFunctionDefineArgList& args;
  NBlock& body;
  NFunctionDefine(int return_type, NIdentifier& name,
                  NFunctionDefineArgList& args, NBlock& body);            
  virtual void generate_Eeyore(Symtab& symt, EeyoreList& el);
};
