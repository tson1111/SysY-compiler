#include <iostream>
#include <sstream>
#include "Node.h"
#include "Tigger.h"

extern NRoot* root;
std::ostream *out;


std::map<int,std::string> token2str;
bool print_S = false, print_e = false, print_t = false;
bool print_file = false;
extern int yyparse();

void init_token2str(){
    token2str[270] = "==";
    token2str[271] = "!=";
    token2str[272] = "<";
    token2str[273] = ">";
    token2str[274] = "&&";
    token2str[275] = "||";
    token2str[279] = "+";
    token2str[280] = "-";
    token2str[281] = "*";
    token2str[282] = "/";
    token2str[283] = "%";
    token2str[284] = "!";
}

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
            else if (std::string("-t") == argv[i]) {
                print_t = true;
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
    init_token2str();
    parse_arg(argc, argv);
    yyparse();
    Symtab symt;
    TiggerList el;
    if (print_t && print_S) {
        root->generate_Tigger(symt, el);
    }
    
    if (out != &std::cout) {
        delete out;
    }
    return 0;
}