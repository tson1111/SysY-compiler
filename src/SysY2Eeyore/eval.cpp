#include "Node.h"
#include <cassert>
#include "sysy.tab.hpp"


// EVALUATION

int NExpression::eval(Symtab& symt) {
    throw std::runtime_error("eval error");
}

int 
NNumber::eval(Symtab& symt){
    return this->value;
}

int 
NIdentifier::eval(Symtab& symt){
    auto val = symt.find_const(this->name);
    return val.value.front();
}

int 
NConditionExpression::eval(Symtab& symt){
    return this->value.eval(symt);
}

int 
NBinaryExpression::eval(Symtab& symt){
    switch (this->op)
    {
    case PLUS: // PLUS
        return lhs.eval(symt) + rhs.eval(symt);
        break;

    case MINUS:
        return lhs.eval(symt) - rhs.eval(symt);
        break;

    case MUL:
        return lhs.eval(symt) * rhs.eval(symt);
        break;

    case DIV:
        return lhs.eval(symt) / rhs.eval(symt);
        break;

    case MOD:
        return lhs.eval(symt) % rhs.eval(symt);
        break;

    case EQ:
        return lhs.eval(symt) == rhs.eval(symt);
        break;

    case NE:
        return lhs.eval(symt) != rhs.eval(symt);
        break;

    case GT:
        return lhs.eval(symt) > rhs.eval(symt);
        break;

    case GE:
        return lhs.eval(symt) >= rhs.eval(symt);
        break;

    case LT:
        return lhs.eval(symt) < rhs.eval(symt);
        break;

    case LE:
        return lhs.eval(symt) <= rhs.eval(symt);
        break;

    case AND:
        return lhs.eval(symt) && rhs.eval(symt);
        break;

    case OR:
        return lhs.eval(symt) || rhs.eval(symt);
        break;
    }
    throw std::runtime_error("Unknow operator");
}

int 
NUnaryExpression::eval(Symtab& symt){
    switch (this->op) {
    case MINUS:
      return -rhs.eval(symt);
      break;

    case NOT:
      return !rhs.eval(symt);
      break;
    }
    throw std::runtime_error("Unknow operator");
}

int 
NCommaExpression::eval(Symtab& symt){
    int ret;
    for (auto &v: values) {
        ret = v->eval(symt);
    }
    return ret;
}

int 
NEvalStatement::eval(Symtab& symt){
    return this->value.eval(symt);
}

int 
NArrayIdentifier::eval(Symtab& symt) {
    auto val = symt.find_const(this->name.name);
    int idx = 0, size = 1;
    for (int i = this->shape.size() + 1; i >=0 ; ++i) {
        idx += this->shape[i]->eval(symt) * size;
        size *= val.shape[i];
    }
    return val.value[idx];
}


