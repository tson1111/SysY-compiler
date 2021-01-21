#include <iostream>
#include <sstream>
#include <map>
#include <fstream>

std::ostream *out;
std::map<char, std::string> token2str;

bool print_S = false, print_e = false, print_t = false;
bool print_file = false;
extern int yyparse();

void init_riscv() {
    token2str['+'] = "add";
    token2str['-'] = "sub";
    token2str['*'] = "mul";
    token2str['/'] = "div";
    token2str['%'] = "rem";
    token2str['<'] = "slt";
    token2str['>'] = "sgt";
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
    parse_arg(argc, argv);
    init_riscv();
    if (print_S) {
        yyparse();
    }
    
    if (out != &std::cout) {
        delete out;
    }
    return 0;
}