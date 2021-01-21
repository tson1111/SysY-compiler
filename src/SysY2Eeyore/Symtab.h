#pragma once
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Var{
public:
  std::vector<int> shape;
  bool is_array;
  std::string name;
  Var(std::string name, bool is_array = false, std::vector<int> shape = {});
};

class Const {
public:
  bool is_array;
  std::vector<int> shape;
  std::vector<int> value;
  Const(std::vector<int> value, bool is_array = false,
            std::vector<int> shape = {});
  Const(int value);
};

class Symtab {
public:
  Symtab();
  using SymbolTable = std::vector<std::unordered_map<std::string, Var>>;
  using ConstTable = std::vector<std::unordered_map<std::string, Const>>;
  using FuncTable = std::unordered_map<std::string, bool>;

  SymbolTable symbol_table = {{}};
  ConstTable const_table = {{}};
  FuncTable func_table = {};

  void insert_symbol(std::string name, Var value);
  void insert_const(std::string name, Const value);
  void insert_func(std::string name_, bool is_void_);

  Var& find_symbol(std::string name);
  Const& find_const(std::string name);
  bool find_func(std::string name);

  void create_scope();
  void end_scope();
};