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
    Interpreter::compilerFuncs.insert({"#entry", [](auto& inter, const auto& args){
        inter.entry = args[0].func->identifier;
        std::cout << inter.entry << " : entry " << '\n';
    }});
    Interpreter::compilerFuncs.insert({"#output", [](auto&, const auto& args){
        for (auto i = 0; i < args.size(); ++i) {
            switch (args[i].type) {
                case Variable::TYPE::INTEGER: std::cout << args[i].i; break;
                case Variable::TYPE::STRING: std::cout << args[i].s; break;
                default: std::cout << "UNHANDLED OUT";
            }
        }
        std::cout << '\n';
    }});

    auto interpreter = Interpreter(ast);
    interpreter.interp();

    return 0;
}
