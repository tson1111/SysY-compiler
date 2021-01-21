#include "Symtab.h"
#include <exception>

Var::Var(std::string name, bool is_array, std::vector<int> shape):
    name(name), shape(shape), is_array(is_array) {}

Const::Const(std::vector<int> value, bool is_array, std::vector<int> shape):
    value(value), shape(shape), is_array(is_array) {}

Const::Const(int value):
    value({value}), shape({}), is_array(false) {}

Symtab::Symtab(){
    insert_func("getint", false);
    insert_func("getch", false);
    insert_func("getarray", false);
    insert_func("putint", true);
    insert_func("putch", true);
    insert_func("putarray", true);
    insert_func("starttime", true);
    insert_func("stoptime", true);
}

void
Symtab::insert_symbol(std::string name, Var value) {
    symbol_table.back().insert({name, value});
}

void
Symtab::insert_const(std::string name, Const value) {
    const_table.back().insert({name, value});
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

Const&
Symtab::find_const(std::string name) {
    int i = const_table.size() - 1;
    for (; i >= 0; --i) {
        auto find = const_table[i].find(name);
        if (find != const_table[i].end()) {
            return find->second;
        }
    }
    throw std::out_of_range("No const called "+ name);
}

void
Symtab::create_scope() {
    symbol_table.push_back({});
    const_table.push_back({});
}

void
Symtab::end_scope() {
    symbol_table.pop_back();
    const_table.pop_back();
}

void
Symtab::insert_func(std::string name_, bool is_void_) {
    func_table.insert({name_, is_void_});
}

bool
Symtab::find_func(std::string name) {
    auto it = func_table.find(name);
    if (it != func_table.end()) {
        return it->second;
    }
    throw std::out_of_range("No function called "+ name);
}