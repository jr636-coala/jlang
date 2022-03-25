#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "parser.hpp"
#include "interpreter.hpp"
#include "c.hpp"

std::string loadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    auto tokeniser = Tokeniser(loadFile("progs/sb.jl"));
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
    Interpreter::compilerFuncs["#loc"] = [](auto&, auto& loc, const auto&){
        Type ret(TypeT::string);
        ret.string->val = loc;
        return ret;
    };
    Interpreter::compilerFuncs["#output"] = [](auto&, auto&, const auto& args) -> Type {
        std::string out;
        for (auto i = 0; i < args.size(); ++i) {
            switch (args[i].type) {
                case TypeT::i64: out = std::to_string(args[i].i64->val); break;
                case TypeT::string: out = args[i].string->val; break;
                default: out = "UNHANDLED OUT";
            }
        }
        JL_C::jl_puts(out.c_str());
    };
    Interpreter::compilerFuncs["#c"] = [](auto&, auto& loc, const auto& args) {
        const auto name = args[0].string->val;
             if (name == "puts") return Type(JL_C::jl_puts(args[1].string->val.c_str()));
        else if (name == "strlen") return Type(JL_C::jl_strlen(args[1].string->val.c_str()));
        else if (name == "fopenw") return Type(JL_C::jl_fopenw(args[1].string->val.c_str()));
        else if (name == "fclose") return Type(JL_C::jl_fclose(*args[1].i64));
        else if (name == "fwrite") return Type(JL_C::jl_fwrite(args[1].string->val.c_str(), *args[2].i64, *args[3].i64));
        else if (name == "system") return Type(JL_C::jl_system(args[1].string->val.c_str()));
        return Type();
    };

    auto interpreter = Interpreter(ast);
    std::cout << *interpreter.interp().i64;

    return 0;
}
