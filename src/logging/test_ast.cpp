#include "ast.hpp"
#include "get_constnum.hpp"

#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <cminus_file>" << std::endl;
    } else {
        auto s = parse(argv[1]);
        auto a = AST(s);
        auto printer = GetConst();
        a.run_visitor(printer);
    }
    return 0;
}
