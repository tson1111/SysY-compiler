#include "Node.h"

bool in_func;
int reg_s_idx, reg_t_idx, reg_a_idx;
int global_cnt, local_cnt;
int param_cnt;
bool is_used[12];
TiggerList func_stmt;

void print(std::string stmt) {
    if (in_func) {
        func_stmt.push_back(stmt);
    }
    else {
        *out << stmt;
    }
}

void
print_func_stmt() {
    for (auto& i: func_stmt) {
        *out << "\t" << i ;
    }
    while (!func_stmt.empty()) {
        func_stmt.pop_back();
    }
}

std::string
find_reg() {
    for (int i = 0; i < 12; ++i) {
        if (!is_used[i]) {
            is_used[i] = true;
            return "s"+std::to_string(i);
        }
    }
    return "t0";
}

void
release_reg(std::string reg) {
    if (reg[0] == 's') 
        is_used[std::stoi(reg.substr(1))] = false;
}

void
get_rightvalue(NRightValue& rl, Symtab& symt, TiggerList& el) {
    auto iden = dynamic_cast<NVariable*>(&rl);
    if (iden) {
        // is var
/*        if (iden->name[0] == 'p') { // param
            std::string reg = find_reg();
            print(reg + " = " + "a" + iden->name.substr(1) + "\n");
            el.push_back(reg);
            return ;
        }*/
        Var tmp = symt.find_symbol(iden->name);
        if (!tmp.is_global) {
            // local
            std::string reg = find_reg();
            print("load " + std::to_string(tmp.tigger_idx) + " " + reg + "\n");
            el.push_back(reg);
        }
        else {
            // global
            std::string reg = find_reg();
            print("load v" + std::to_string(tmp.tigger_idx) + " " + reg + "\n");
            el.push_back(reg);
        }
    }
    else {
        // is num
        auto num = dynamic_cast<NNumber*>(&rl);
        std::string reg = find_reg();
        print(reg + " = " + std::to_string(num->value) + "\n");
        el.push_back(reg);
    }
}

void
store_variable(NVariable& rl, Symtab& symt, TiggerList& el, std::string reg) {
    Var tmp = symt.find_symbol(rl.name);
    if (tmp.is_global) {
        // global
        std::string tmp_reg = find_reg();
        print("loadaddr v" + std::to_string(tmp.tigger_idx) + " " + tmp_reg + "\n");
        print(tmp_reg + "[0] = " + reg + "\n");
        release_reg(tmp_reg);
    }
    else {
        // local
        print("store " + reg + " " + std::to_string(tmp.tigger_idx) + "\n");
    }
    release_reg(reg);
}

void
get_array_exp(NArrayExpression* arr, Symtab& symt, TiggerList& el) {
    get_rightvalue(arr->idx, symt, el);
    std::string reg_idx = el.back();
    el.pop_back();
    
    if ((arr->name).name[0] == 'p') { //param
        get_rightvalue(arr->name, symt, el);
        std::string reg = el.back();
        el.pop_back();
        print(reg + " = " + reg + " + " + reg_idx + "\n");
        print(reg_idx + " = " + reg + "[0]\n");
        el.push_back(reg_idx);
        release_reg(reg);
        return ;
    }
    Var arr_info = symt.find_symbol((arr->name).name);
    std::string reg = find_reg();
    
    if (arr_info.is_global) {
        // global
        print("loadaddr v" + std::to_string(arr_info.tigger_idx) + " " + reg + "\n");
    }
    else {
        // local
        print("loadaddr " + std::to_string(arr_info.tigger_idx) + " " + reg + "\n");

    }
    print(reg + " = " + reg + " + " + reg_idx + "\n");
    print(reg_idx + " = " + reg + "[0]\n");
    el.push_back(reg_idx);
    release_reg(reg);
}

void
store_array_exp(NArrayExpression* arr, Symtab& symt, TiggerList& el, std::string reg) {
    get_rightvalue(arr->idx, symt, el);
    std::string reg_idx = el.back();
    el.pop_back();
    std::string addr;

    if (arr->name.name[0] == 'p') {
        get_rightvalue(arr->name, symt, el);
        addr = el.back();
        el.pop_back();
    }
    else {
        Var arr_info = symt.find_symbol((arr->name).name);

        addr = find_reg();
        if (arr_info.is_global) {
            // global
            print("loadaddr v" + std::to_string(arr_info.tigger_idx) + " " + addr + "\n");
        }
        else {
            // local
            print("loadaddr " + std::to_string(arr_info.tigger_idx) + " " + addr + "\n");
        }
    }
    print(addr + " = " + addr + " + " + reg_idx + "\n");
    print(addr + "[0] = " + reg + "\n");
    release_reg(addr);
    release_reg(reg_idx);
    release_reg(reg);
}


void
NRoot::generate_Tigger(Symtab& symt, TiggerList& el) {
    symt.create_scope();
    in_func = false;
    for (auto& i: this->body) {
        i->generate_Tigger(symt, el);
    }
    *out << "\n";
    symt.end_scope();
}

void
NFuncDefStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    for (auto& i: this->body) {
        i->generate_Tigger(symt, el);
    }
}

void
NVarDeclare::generate_Tigger(Symtab& symt, TiggerList& el) {
    if (in_func) {
        // local var
        Var tmp(name.name);
        tmp.tigger_idx = local_cnt++;
        symt.insert_symbol(name.name, tmp);
    }
    else {
        // global var
        Var tmp(name.name, false, 4, true);
        tmp.tigger_idx = global_cnt++;
        symt.insert_symbol(name.name, tmp);
        print("v" + std::to_string(tmp.tigger_idx) + " = 0 \n");
    }
}

void
NArrayDeclare::generate_Tigger(Symtab& symt, TiggerList& el) {
    if (in_func) {
        // local var
        Var tmp(name.name, true, size, false);
        tmp.tigger_idx = local_cnt;
        symt.insert_symbol(name.name, tmp);
        local_cnt += size/4;
    }
    else {
        // global var
        Var tmp(name.name, true, size, true);
        tmp.tigger_idx = global_cnt++;
        symt.insert_symbol(name.name, tmp);
        print("v" + std::to_string(tmp.tigger_idx) + " = malloc " + std::to_string(size) + "\n");
    }

}

void
NFunctionDefine::generate_Tigger(Symtab& symt, TiggerList& el) {
    symt.create_scope();
    in_func = true;
    local_cnt = 0;
    Func tmp(name.name, param_num);
    symt.insert_func(name.name, tmp);
    for (int i = 0; i < param_num; ++i) {
        print("store a" + std::to_string(i) + " " + std::to_string(local_cnt++) + "\n");
        Var tmp("p"+std::to_string(i));
        tmp.tigger_idx = i;
        symt.insert_symbol("p"+std::to_string(i), tmp);
    }
    stmt.generate_Tigger(symt, el);
    *out << name.name << " " << "[" << param_num << "] [" << local_cnt << "]\n";
    print_func_stmt();
    *out << "end " << name.name << "\n";
    in_func = false;
    symt.end_scope();
} 


void
NFuncCallAssignment::generate_Tigger(Symtab& symt, TiggerList& el) {
    param_cnt = 0;
    print("call " + func.name + "\n");
    store_variable(var, symt, el, "a0");
}

void
NFunctionCall::generate_Tigger(Symtab& symt, TiggerList& el) {
    param_cnt = 0;
    print("call " + name.name + "\n");
}

void
NAssignment::generate_Tigger(Symtab& symt, TiggerList& el) {
    // get the reg of rhs
    auto rv = dynamic_cast<NRightValue*>(&rhs);
    if (rv) {
        // rhs = rightvalue
        get_rightvalue(*rv, symt, el);

    }
    else {
        auto arr = dynamic_cast<NArrayExpression*>(&rhs);
        if (arr) {
            // rhs = array exp
            get_array_exp(arr, symt, el);
        }
        else {
            // rhs = binary exp or unary exp
            rhs.generate_Tigger(symt, el);
        }
    }
    std::string reg_rhs = el.back();
    el.pop_back();

    // store to lhs
    auto var = dynamic_cast<NVariable*>(&lhs);
    if (var) {
        // lhs = var
        store_variable(*var, symt, el, reg_rhs);
    }
    else {
        // lhs = array exp
        auto arr = dynamic_cast<NArrayExpression*>(&lhs);
        store_array_exp(arr, symt, el, reg_rhs);
    }
}

void
NArrayExpression::generate_Tigger(Symtab& symt, TiggerList& el) {}

void
NBinaryExpression::generate_Tigger(Symtab& symt, TiggerList& el) {
    get_rightvalue(rhs, symt, el);
    std::string reg2 = el.back();
    el.pop_back();
    get_rightvalue(lhs, symt, el);
    std::string reg1 = el.back();
    el.pop_back();
    print(reg1 + " = " + reg1 + " " + token2str[op] + " " + reg2 + "\n");
    el.push_back(reg1);
    release_reg(reg2);
}

void
NLogicExpression::generate_Tigger(Symtab& symt, TiggerList& el) {
    get_rightvalue(rhs, symt, el);
    std::string reg2 = el.back();
    el.pop_back();
    get_rightvalue(lhs, symt, el);
    std::string reg1 = el.back();
    el.pop_back();
    el.push_back(reg1);
    el.push_back(reg2);
    el.push_back(reg1 + " " + token2str[op] + " " + reg2);
}

void
NUnaryExpression::generate_Tigger(Symtab& symt, TiggerList& el) {
    get_rightvalue(rhs, symt, el);
    std::string reg = el.back();
    el.pop_back();
    print(reg + " = " + token2str[op] + reg + "\n");
    el.push_back(reg);
}

void
NGotoStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    print("goto " + l.name + "\n");
}

void
NConditionStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    cond.generate_Tigger(symt, el);
    std::string tmp = el.back();
    el.pop_back();
    print("if " + tmp + " ");
    std::string tmp1 = el.back();
    el.pop_back();
    std::string tmp2 = el.back();
    el.pop_back();
    release_reg(tmp1);
    release_reg(tmp2);
    l.generate_Tigger(symt, el);
}

void
NLabelStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    print(l.name + ":\n");
}

void
NParamStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    get_rightvalue(rl, symt, el);
    std::string reg = el.back();
    el.pop_back();
    print("a" + std::to_string(param_cnt++) + " = " + reg + "\n");
    release_reg(reg);
}

void
NReturnStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    get_rightvalue(rl, symt, el);
    std::string reg = el.back();
    el.pop_back();
    print("a0 = " + reg + "\n");
    release_reg(reg);
    print("return\n");
}

void
NReturnVoidStatement::generate_Tigger(Symtab& symt, TiggerList& el) {
    print("return\n");
}

void
NNumber::generate_Tigger(Symtab& symt, TiggerList& el) {}

void
NLabel::generate_Tigger(Symtab& symt, TiggerList& el) {}

void
NVariable::generate_Tigger(Symtab& symt, TiggerList& el) {}

void
NFunction::generate_Tigger(Symtab& symt, TiggerList& el) {}
