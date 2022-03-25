#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "parser.hpp"
#include "interpreter.hpp"

std::string loadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    auto tokeniser = Tokeniser(loadFile("progs/ns1.jl"));
    auto tokens = tokeniser.getTokens();
    //printTokens(tokens);
    auto parser = Parser(tokens);
    auto ast = parser.parse();
    Parser::printAST(ast);
    std::cout << "\n\nPROGRAM OUTPUT:\n";

    // Define compilerFuncs
    Interpreter::compilerFuncs["#entry"] = [](auto& inter, auto&, const auto& args) -> Type {
        inter.entry = args[0].fn->val->identifier;
        std::cout << inter.entry << " : entry " << '\n';
    };
    Interpreter::compilerFuncs["#output"] = [](auto&, auto&, const auto& args) -> Type {
        for (auto i = 0; i < args.size(); ++i) {
            switch (args[i].type) {
                case TypeT::i32: std::cout << args[i].i32->val; break;
                case TypeT::string: std::cout << args[i].string->val; break;
                default: std::cout << "UNHANDLED OUT";
            }
        }
        std::cout << '\n';
    };
    Interpreter::compilerFuncs["#loc"] = [](auto&, auto& loc, const auto&){
        Type ret(TypeT::string);
        ret.string->val = loc;
        return ret;
    };

    auto interpreter = Interpreter(ast);
    interpreter.interp();

    return 0;
}
