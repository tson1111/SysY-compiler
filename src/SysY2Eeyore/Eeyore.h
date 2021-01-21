#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <stack>
#include "Node.h"
#include "Symtab.h"

using EeyoreList = std::vector<std::string>;

extern std::ostream* out;
extern std::map<int,std::string> token2str;
void init_eeyore();
