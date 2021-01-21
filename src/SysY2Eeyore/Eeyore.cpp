#include "Node.h"
#include "Eeyore.h"
#include "Symtab.h"
#include "sysy.tab.hpp"


int T_idx, t_idx, p_idx, l_idx;
EeyoreList stmt_list, global_list;
bool in_func = false, have_return = false;
//#define DEBUG
void init_eeyore(){
    T_idx = t_idx = p_idx = l_idx = 0;
    token2str[EQ] = "==";
    token2str[NE] = "!=";
    token2str[LT] = "<";
    token2str[LE] = "<=";
    token2str[GT] = ">";
    token2str[GE] = ">=";
    token2str[AND] = "&&";
    token2str[OR] = "||";
    token2str[PLUS] = "+";
    token2str[MINUS] = "-";
    token2str[MUL] = "*";
    token2str[DIV] = "/";
    token2str[MOD] = "%";
    token2str[NOT] = "!";
    stmt_list = {};
    global_list = {};
}

void print(std::string stmt) {
    #ifdef DEBUG
    *out << stmt;
    #endif
    if (in_func) {
        stmt_list.push_back(stmt);
    }
    else {
        if (stmt.find("var") != std::string::npos) {
            *out << stmt;
        }
        else {
            global_list.push_back(stmt);
        }
    }
    
}

void gen_code() {
    for (auto& i: stmt_list) {
        if (i.substr(0,3) == std::string("var")) {
            *out << i;
        }
    }
    for (auto& i: stmt_list) {
        if (i.substr(0,3) != std::string("var")) {
            *out << i;
        }
    }
    while (!stmt_list.empty()) {
        stmt_list.pop_back();
    }
}

void gen_global_code() {
    for (auto &i: global_list) {
        *out << i;
    }
    while (!stmt_list.empty()) {
        global_list.pop_back();
    }
}

void
NRoot::generate_Eeyore(Symtab& symt, EeyoreList& el){
    symt.create_scope();
    for (auto& i: this->body) {
        i->generate_Eeyore(symt, el);
    }
    symt.end_scope();
}

void
NFunctionDefine::generate_Eeyore(Symtab& symt, EeyoreList& el){
    symt.create_scope();
    have_return = false;
    p_idx = 0;
    int arg_len = this->args.list.size();
    *out << "f_" << (this->name).name << " [" << std::to_string(arg_len) << "]\n";
    if (this->name.name == "main") {
        gen_global_code();
    }
    bool is_void = (return_type == 269);
    in_func = true;
    symt.insert_func(this->name.name, is_void);
    this->args.generate_Eeyore(symt, el);
    this->body.generate_Eeyore(symt, el);
    if (!have_return) {
        print("return \n");
    }
    gen_code();
    in_func = false;
    *out << "end f_" << (this->name).name << "\n";
    symt.end_scope();
}

void 
NFunctionDefineArgList::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    for (auto& i: list) {
        i->generate_Eeyore(symt, el);
    }
}

void 
NFunctionDefineArg::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    auto iden = dynamic_cast<NArrayIdentifier*>(&name);
    if (iden) {
        //is array
        std::vector<int> shape;
        for (auto i: iden->shape) {
            shape.push_back(i->eval(symt));
        }
        std::string tmpname = "p" + std::to_string(p_idx++);
        symt.insert_symbol(this->name.name, Var(tmpname, true, shape));
    }
    else {
        std::string tmpname = "p" + std::to_string(p_idx++);
        symt.insert_symbol(this->name.name, Var(tmpname));
    }
}

void
NReturnStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    have_return = true;
    if (value == NULL) {
        print("return \n");
        return ; 
    }
    this->value->generate_Eeyore(symt, el);
    auto tmp = el.back();
    el.pop_back();
    print("return " + tmp + "\n");
}

void
NBlock::generate_Eeyore(Symtab& symt, EeyoreList& el){
    symt.create_scope();
    for (auto& i: this->statements) {
        i->generate_Eeyore(symt, el);
    }
    symt.end_scope();
}

void
NAssignment::generate_Eeyore(Symtab& symt, EeyoreList& el){
    auto tmp_symbol = symt.find_symbol(lhs.name);
    this->rhs.generate_Eeyore(symt, el);
    auto r = el.back();
    el.pop_back();
    if (tmp_symbol.is_array) {
        // is_array
        auto array_lhs = dynamic_cast<NArrayIdentifier*>(&lhs);
        for (auto& i: array_lhs->shape) {
            i->generate_Eeyore(symt, el);
        }
        int dim = tmp_symbol.shape.size();
        int base = 4;
        std::string final = "t" + std::to_string(t_idx++);
        print("var " + final + "\n");
        print(final + " = 0\n");
        for (int i = 0; i < dim; ++i) {
            std::string tmpname = "t" + std::to_string(t_idx++);
            print("var " + tmpname + "\n");
            std::string off = el.back();
            el.pop_back();
            print(tmpname + " = " + off + " * " + std::to_string(base) + "\n");
            print(final + " = " + final + " + " + tmpname + "\n");
            base *= tmp_symbol.shape[dim - 1 - i];
        }
        print(tmp_symbol.name + "[" + final + "] = " + r + "\n");
    }
    else {
        print(tmp_symbol.name + " = " + r + "\n");
    }
}

void
NIfElseStatement::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    cond.generate_Eeyore(symt, el);
    std::string op = el.back();
    el.pop_back();
    std::string l_true = "l" + std::to_string(l_idx++);
    std::string l_false = "l" + std::to_string(l_idx++);
    std::string l_merge = "l" + std::to_string(l_idx++);
    print("if " + op + " != 0 goto " + l_true + "\n");
    print("goto " + l_false + "\n");
    print(l_true + ":\n");
    thenstmt.generate_Eeyore(symt, el);
    print("goto " + l_merge + "\n");
    print(l_false + ":\n");
    elsestmt.generate_Eeyore(symt, el);
    print(l_merge + ":\n");
}

void
NIfStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    cond.generate_Eeyore(symt, el);
    std::string op = el.back();
    el.pop_back();
    std::string l_true = "l" + std::to_string(l_idx++);
    std::string l_false = "l" + std::to_string(l_idx++);
    print("if " + op + " != 0 goto " + l_true + "\n");
    print("goto " + l_false + "\n");
    print(l_true + ":\n");
    thenstmt.generate_Eeyore(symt, el);
    print(l_false + ":\n");
}

void
NWhileStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::string l_cond = "l" + std::to_string(l_idx++);
    std::string l_merge = "l" + std::to_string(l_idx++);
    el.push_back(l_cond);
    el.push_back(l_merge);
    print(l_cond + ":\n");
    cond.generate_Eeyore(symt, el);
    std::string op = el.back();
    el.pop_back();
    print("if " + op + " == 0 goto " + l_merge + "\n");
    dostmt.generate_Eeyore(symt, el);
    print("goto " + l_cond + "\n");
    print(l_merge + ":\n");
}

void
NBreakStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::string l_merge = el.back();
    print("goto " + l_merge + "\n");
}

void
NContinueStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::string l_merge = el.back();
    el.pop_back();
    std::string l_cond = el.back();
    el.pop_back();
    print("goto " + l_cond + "\n");
    el.push_back(l_cond);
    el.push_back(l_merge);
}

void
NConditionExpression::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    value.generate_Eeyore(symt, el);
}

void
NEvalStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    this->value.generate_Eeyore(symt, el);
}

void
NDeclareStatement::generate_Eeyore(Symtab& symt, EeyoreList& el){
    for (auto& i: this->list) {
        i->generate_Eeyore(symt, el);
    }
}

void
Node::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    return ;
}


void
NVarDeclareWithInit::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::string tmpname = "T" + std::to_string(T_idx++);
    symt.insert_symbol(this->name.name, Var(tmpname));
    print("var " + tmpname + "\n");
    NAssignment(this->name, this->value).generate_Eeyore(symt, el);
    if (is_const) {
        int k = this->value.eval(symt);
        symt.insert_const(this->name.name, Const(k));
    }
}

void
NVarDeclare::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::string tmpname = "T" + std::to_string(T_idx++);
    symt.insert_symbol(this->name.name, Var(tmpname));
    print("var " + tmpname + "\n");
}

void
NArrayDeclareWithInit::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::vector<int> shape;
    for (auto &i: this->name.shape) {
        shape.push_back(i->eval(symt));
    }
    int size = 1;
    for (auto i: shape) {
        size *= i;
    }
    std::string tmpname = "T" + std::to_string(T_idx++);
    symt.insert_symbol(this->name.name.name, Var(tmpname, true, shape));
    print("var " + std::to_string(size*4) + ' ' + tmpname + "\n");
    // TODO: get values into vector<int> "init_values";
    if (is_const) {
        // const array
        std::vector<int> init_values(size);
        bool all_is_number = true;
        for (auto i: value.value_list) {
            if (!i->is_number) {
                all_is_number = false;
                break;
            }
        }
        if (all_is_number) {
            // init: int a[] = {int*};
            int cnt = 0;
            for (auto i: value.value_list) {
                init_values.push_back(i->value->eval(symt));
                ++cnt;
            }
            for (int i = 0; i < size - cnt; ++i) {
                init_values.push_back(0);
            }
        }
        else {
            for (int i = 0; i < size; ++i) {
                init_values.push_back(value.value_list[i]->value->eval(symt));
            }            
        }
        for (int i = 0; i < size; ++i) {
            print(tmpname + "[" + std::to_string(4*i) + "] = " + std::to_string(init_values[i]) + "\n");
        }
        symt.insert_const(this->name.name.name, Const(init_values, true, shape));
    }
    else {
        // var array
        std::vector<std::string> init_values(size);
        bool all_is_number = true;
        for (auto i: value.value_list) {
            if (!i->is_number) {
                all_is_number = false;
                break;
            }
        }
        if (all_is_number) {
            // init: int a[] = {int*};
            int cnt = 0;
            for (auto i: value.value_list) {
                i->value->generate_Eeyore(symt, el);
                ++cnt;
            }
            for (int i = 0; i < cnt; ++i) {
                std::string tmp = el.back();
                el.pop_back();
                init_values[cnt - 1 - i] = tmp;
            }
            for (int i = 0; i < size - cnt; ++i) {
                init_values.push_back("0");
            }
        }
        else {
            for (int i = 0; i < size; ++i) {
                value.value_list[i]->value->generate_Eeyore(symt, el);
            }
            for (int i = 0; i < size; ++i) {
                std::string tmp = el.back();
                el.pop_back();
                init_values[size - 1 - i] = tmp;
            }
        }
        for (int i = 0; i < size; ++i) {
            print(tmpname + "[" + std::to_string(4*i) + "] = " + init_values[i] + "\n");
        }
    }
}

void
NArrayDeclare::generate_Eeyore(Symtab& symt, EeyoreList& el){
    std::vector<int> shape;
    for (auto &i: this->name.shape) {
        shape.push_back(i->eval(symt));
    }
    int size = 1;
    for (auto i:shape) {
        size *= i;
    }
    size *= 4;
    std::string tmpname = "T" + std::to_string(T_idx++);
    symt.insert_symbol(this->name.name.name, Var(tmpname, true, shape));
    print("var " + std::to_string(size) + ' ' + tmpname + "\n");
}

void
NNumber::generate_Eeyore(Symtab& symt, EeyoreList& el){
    el.push_back(std::to_string(this->value));
}

void
NBinaryExpression::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    lhs.generate_Eeyore(symt, el);
    auto l = el.back();
    el.pop_back();
    rhs.generate_Eeyore(symt, el);
    auto r = el.back();
    el.pop_back();
    std::string tmp = "t"+std::to_string(t_idx++);
    print("var " + tmp + "\n");
    if (token2str[op] == "<=") {
        std::string tmp_lt = "t"+std::to_string(t_idx++);
        print("var " + tmp_lt + "\n");
        std::string tmp_eq = "t"+std::to_string(t_idx++);
        print("var " + tmp_eq + "\n");
        print(tmp_lt + " = " + l + " < " + r + "\n");
        print(tmp_eq + " = " + l + " == " + r + "\n");
        print(tmp + " = " + tmp_lt + " || " + tmp_eq + "\n");
        el.push_back(tmp);
    }
    
    else if (token2str[op] == ">=") {
        std::string tmp_gt = "t"+std::to_string(t_idx++);
        print("var " + tmp_gt + "\n");
        std::string tmp_eq = "t"+std::to_string(t_idx++);
        print("var " + tmp_eq + "\n");
        print(tmp_gt + " = " + l + " > " + r + "\n");
        print(tmp_eq + " = " + l + " == " + r + "\n");
        print(tmp + " = " + tmp_gt + " || " + tmp_eq + "\n");
        el.push_back(tmp);
    }
    else {
        print(tmp + " = " + l + " " + token2str[op] + " " + r + "\n");
        el.push_back(tmp);
    }
}


void
NIdentifier::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    auto tmp = symt.find_symbol(name).name;
    el.push_back(tmp);
}

void
NFunctionCallArgList::generate_Eeyore(Symtab& symt, EeyoreList& el){
    for (auto& i: args) {
        i->generate_Eeyore(symt, el);
    }
    int N = args.size();
    int k = el.size();
    for (int i = 0; i < N; ++i) {
        print("param " + el[k-(N-i)] + "\n");
    }
    for (int i = 0; i < N; ++i) {
        el.pop_back();
    }

}

void
NFunctionCall::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    args.generate_Eeyore(symt, el);
    if (symt.find_func(this->name.name)) {
        // function type == void
        print("call f_" + this->name.name + "\n");
    }
    else {
        std::string tmp = "t" + std::to_string(t_idx++);
        print("var " + tmp + "\n");
        print(tmp + " = call f_" + this->name.name + "\n");
        el.push_back(tmp);
    }
}

void
NUnaryExpression::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    std::string tmp = "t" + std::to_string(t_idx++);
    rhs.generate_Eeyore(symt, el);
    auto r = el.back();
    el.pop_back();
    print("var " + tmp + "\n");
    print(tmp + " = " + token2str[op] + r + "\n");
    el.push_back(tmp);
}

void
NArrayIdentifier::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    auto tmp_symbol = symt.find_symbol(name.name);
    for (auto& i: shape) {
        i->generate_Eeyore(symt, el);
    }
    int dim = tmp_symbol.shape.size();
    int base = 4;
    std::string final = "t" + std::to_string(t_idx++);
    print("var " + final + "\n");
    print(final + " = 0\n");
    for (int i = 0; i < dim; ++i) {
        std::string tmpname = "t" + std::to_string(t_idx++);
        print("var " + tmpname + "\n");
        std::string off = el.back();
        el.pop_back();
        print(tmpname + " = " + off + " * " + std::to_string(base) + "\n");
        print(final + " = " + final + " + " + tmpname + "\n");
        base *= tmp_symbol.shape[dim - 1 - i];
    }
    std::string tmpname = "t" + std::to_string(t_idx++);
    print("var " + tmpname + "\n");
    print(tmpname + " = " + tmp_symbol.name + "[" + final + "]" + "\n");
    el.push_back(tmpname);
}

void
NVoidStatement::generate_Eeyore(Symtab& symt, EeyoreList& el) {
    return ;
}