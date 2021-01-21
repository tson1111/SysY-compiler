#include "Symtab.h"
#include <exception>

Var::Var(std::string name, bool is_array, int shape, bool is_global):
    name(name), shape(shape), is_array(is_array), is_global(is_global) {}

Func::Func(std::string name,  int param_cnt):
    name(name), is_void(false), param_cnt(param_cnt), stack_space(0) {}


Symtab::Symtab(){
    insert_func("getint", Func("getint", 0));
    insert_func("getch", Func("getch", 0));
    insert_func("getarray", Func("getarray", 0));
    insert_func("putint", Func("putint", 1));
    insert_func("putch", Func("putch", 1));
    insert_func("putarray", Func("putarray", 1));
    insert_func("starttime", Func("starttime", 0));
    insert_func("stoptime", Func("stoptime", 0));
}

void
Symtab::insert_symbol(std::string name, Var value) {
    symbol_table.back().insert({name, value});
}


void
Symtab::insert_func(std::string name_, Func value) {
    func_table.insert({name_, value});
}

Var&
Symtab::find_symbol(std::string name) {
    int i = symbol_table.size() - 1;
    for (; i >= 0; --i) {
        auto find = symbol_table[i].find(name);
        if (find != symbol_table[i].end()) {
            return find->second;
        }
    }
    throw std::out_of_range("No symbol called "+ name);
}

Func&
Symtab::find_func(std::string name) {
    auto it = func_table.find(name);
    if (it != func_table.end()) {
        return it->second;
    }
    throw std::out_of_range("No function called "+ name);
}

void
Symtab::create_scope() {
    symbol_table.push_back({});
}

void
Symtab::end_scope() {
    symbol_table.pop_back();
}