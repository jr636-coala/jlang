#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "parser.hpp"
#include "interpreter.hpp"
#include "runtime.hpp"


std::string loadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    auto path = argv[1];
    auto tokeniser = Tokeniser(loadFile(path), path);
    auto tokens = tokeniser.getTokens();
    //Tokeniser::printTokens(tokens);
    auto parser = Parser(tokens);
    auto ast = parser.parse();
    std::cout << ast.params[0].type;
    std::cout << "\n\nPROGRAM OUTPUT:\n";

    // Define compilerFuncs
    Interpreter::compilerFuncs["#entry"] = [](auto& inter, auto&, const auto& args) -> TypeVal {
        inter.entry = args[0].fn->val;
        std::cout << inter.entry.identifier.name << " : entry " << '\n';
    };
    Interpreter::compilerFuncs["#loc"] = [](auto&, auto& loc, const auto&){
        TypeVal ret(TypeT::string);
        ret.string->val = loc;
        return ret;
    };
    Interpreter::compilerFuncs["#output"] = [](auto&, auto&, const auto& args) -> TypeVal {
        std::string out;
        for (auto i = 0; i < args.size(); ++i) {
            switch (args[i].type) {
                case TypeT::i64: out += std::to_string(args[i].i64->val); break;
                case TypeT::string: out += args[i].string->val; break;
                default: out += "UNHANDLED OUT";
            }
        }
        std::cout << out << '\n';
    };
    Interpreter::compilerFuncs["#c"] = [](auto&, auto& loc, const auto& args) {
        const auto name = args[0].string->val;
             if (name == "puts")   return JL_C::jl_puts(args[1]);
        else if (name == "strlen") return JL_C::jl_strlen(args[1]);
        else if (name == "fopenw") return JL_C::jl_fopenw(args[1]);
        else if (name == "fclose") return JL_C::jl_fclose(args[1]);
        else if (name == "fwrite") return JL_C::jl_fwrite(args[1], args[2]);
        else if (name == "system") return JL_C::jl_system(args[1]);
        return TypeVal();
    };

    Interpreter::compilerFuncs["#module"] = [](auto& interp, auto&, const auto& args) {
        auto path =
                std::filesystem::path(interp.filename).parent_path().string()
                + "/"
                + args[args.size() > 1 ? 1 : 0].string->val;
        auto tokens = Tokeniser(loadFile(path), path).getTokens();
        auto ast = Parser(tokens).parseModule();
        auto module = Interpreter(ast, path).interpModule();
        if (args.size() > 1) {
            auto name = args[0].string->val;
            interp.currentScope->val[name] = module;
        }
        return module;
    };

    auto interpreter = Interpreter(ast, path);
    interpreter.interp();

    return 0;
}
