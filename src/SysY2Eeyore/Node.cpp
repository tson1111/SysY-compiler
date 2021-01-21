#include "Node.h"

Node::~Node() {}

NIdentifier::NIdentifier(const std::string& name_) : name(name_) {}

NConditionExpression::NConditionExpression(NExpression& value) : value(value) {}

NBinaryExpression::NBinaryExpression(NExpression& lhs, int op, NExpression& rhs)
    : lhs(lhs), rhs(rhs), op(op) {}

NUnaryExpression::NUnaryExpression(int op, NExpression& rhs)
    : rhs(rhs), op(op) {}


NFunctionCall::NFunctionCall(NIdentifier& name, NFunctionCallArgList& args)
    : name(name), args(args) {}

NNumber::NNumber(const std::string& value_) : value(std::stoi(value_, 0, 0)) {}
NNumber::NNumber(INTEGER value_) : value(value_) {}

NAssignment::NAssignment(NIdentifier& lhs, NExpression& rhs)
    : lhs(lhs), rhs(rhs) {}

NIfElseStatement::NIfElseStatement(NConditionExpression& cond,
                                   NStatement& thenstmt, NStatement& elsestmt)
    : cond(cond), thenstmt(thenstmt), elsestmt(elsestmt) {}

NIfStatement::NIfStatement(NConditionExpression& cond, NStatement& thenstmt)
    : cond(cond), thenstmt(thenstmt) {}

NWhileStatement::NWhileStatement(NConditionExpression& cond, NStatement& dostmt)
    : cond(cond), dostmt(dostmt) {}

NReturnStatement::NReturnStatement(NExpression* value) : value(value) {}

NDeclareStatement::NDeclareStatement(int type) : type(type){};

NArrayDeclareInitValue::NArrayDeclareInitValue(bool is_number,
                                               NExpression* value)
    : is_number(is_number), value(value) {}

NVarDeclareWithInit::NVarDeclareWithInit(NIdentifier& name, NExpression& value,
                                         bool is_const)
    : name(name), value(value), is_const(is_const) {}

NVarDeclare::NVarDeclare(NIdentifier& name) : name(name) {}

NArrayIdentifier::NArrayIdentifier(NIdentifier& name)
    : NIdentifier(name), name(name) {}

NArrayDeclareWithInit::NArrayDeclareWithInit(NArrayIdentifier& name,
                                             NArrayDeclareInitValue& value,
                                             bool is_const)
    : name(name), value(value), is_const(is_const) {}

NArrayDeclare::NArrayDeclare(NArrayIdentifier& name) : name(name) {}

NFunctionDefineArg::NFunctionDefineArg(int type, NIdentifier& name)
    : type(type), name(name) {}

NFunctionDefine::NFunctionDefine(int return_type, NIdentifier& name,
                                 NFunctionDefineArgList& args, NBlock& body)
    : return_type(return_type), name(name), args(args), body(body) {}

NEvalStatement::NEvalStatement(NExpression& value):value(value) {}

NCommaExpression::NCommaExpression(){}

