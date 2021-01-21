#include <iostream>
#include <sstream>
#include "Node.h"
#include "Eeyore.h"

extern NRoot* root;
std::ostream *out;

std::map<int,std::string> token2str;
bool print_S = false, print_e = false;
bool print_file = false;
extern int yyparse();

void parse_arg(int argc, char** argv) {
    char* in = NULL;
    int s = 0;
    out = &std::cout;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (std::string("-S") == argv[i]) {
                print_S = true;
            }
            else if (std::string("-e") == argv[i]) {
                print_e = true;
            }
            else if (std::string("-o") == argv[i]) {
                print_file = true;
            }
        }
        else {
            if (print_file) {
                out = new std::ofstream(argv[i], std::ofstream::out);
            }
            else {
                in = argv[i];
            }
            print_file = false;
        }
    }
    if (in != NULL) freopen(in, "r", stdin);
}

int main(int argc, char* argv[]) {
    // Usage:./compiler -S [-e|-t] in.sy -o out.S
    init_eeyore();
    parse_arg(argc, argv);
    yyparse();
    Symtab symt;
    EeyoreList el;
    if (print_e && print_S) {
        root->generate_Eeyore(symt, el);
    }
    
    if (out != &std::cout) {
        delete out;
    }
    return 0;
}