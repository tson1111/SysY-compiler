#pragma once
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Var{
public:
  int shape;
  bool is_array;
  std::string name;
  int tigger_idx;
  bool is_global;
  Var(std::string name, bool is_array = false, int shape = 4, bool is_global = false);
};

class Func{
public:
    bool is_void;
    int param_cnt;
    std::string name;
    int stack_space;
    Func(std::string name, int param_cnt = 0);
};



class Symtab {
public:
  Symtab();
  using SymbolTable = std::vector<std::unordered_map<std::string, Var>>;
  using FuncTable = std::unordered_map<std::string, Func>;

  SymbolTable symbol_table = {{}};
  FuncTable func_table = {};

  void insert_symbol(std::string name, Var value);
  void insert_func(std::string name_, Func value);

  Var& find_symbol(std::string name);
  Func& find_func(std::string name);

  void create_scope();
  void end_scope();
};