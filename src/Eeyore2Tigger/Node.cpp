#include "Node.h"
#include "Tigger.h"

Node::~Node() {}

NVariable::NVariable(const std::string& name_) : name(name_) {}
NFunction::NFunction(const std::string& name_) : name(name_) {}
NLabel::NLabel(const std::string& name_) : name(name_) {}

NNumber::NNumber(const std::string& value_) : value(std::stoi(value_, 0, 0)) {}
NNumber::NNumber(std::int32_t value_) : value(value_) {}

NVarDeclare::NVarDeclare(NVariable& name_) : name(name_) {}

NArrayDeclare::NArrayDeclare(NVariable& name_, NNumber& num): name(name_), size(num.value) {}

NFunctionDefine::NFunctionDefine(NFunction &name_, NNumber &num_, NFuncDefStatement& stmt_): name(name_), param_num(num_.value), stmt(stmt_) {} 

NFunctionCall::NFunctionCall(NFunction& name_): name(name_) {}

NAssignment::NAssignment(NExpression& lhs_, NExpression& rhs_): lhs(lhs_), rhs(rhs_) {}

NArrayExpression::NArrayExpression(NVariable& n, NRightValue& i): name(n), idx(i) {}

NBinaryExpression::NBinaryExpression(NRightValue& lhs_, int op_, NRightValue& rhs_): lhs(lhs_), op(op_), rhs(rhs_) {}

NUnaryExpression::NUnaryExpression(int op_, NRightValue& rhs_): op(op_), rhs(rhs_) {}

NGotoStatement::NGotoStatement(NLabel& l_): l(l_) {}

NConditionStatement::NConditionStatement(NLogicExpression& cond_, NGotoStatement& l_):cond(cond_), l(l_) {}

NLabelStatement::NLabelStatement(NLabel& l_): l(l_) {}

NParamStatement::NParamStatement(NRightValue& rl_): rl(rl_) {}

NReturnStatement::NReturnStatement(NRightValue& rl_): rl(rl_) {}

NReturnVoidStatement::NReturnVoidStatement() {}

NFuncCallAssignment::NFuncCallAssignment(NVariable& var_, NFunction& func_):var(var_), func(func_) {}

NLogicExpression::NLogicExpression(NRightValue& lhs_, int op_, NRightValue& rhs_): lhs(lhs_), op(op_), rhs(rhs_) {}