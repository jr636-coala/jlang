//
// Created by glover on 08/03/2022.
//

#ifndef LANG_INTERPRETER_HPP
#define LANG_INTERPRETER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "parser.hpp"
#include "type.hpp"

using Scope = TypeI<TypeT::ns>;

class Interpreter {
public:
    Interpreter(auto program, const auto& filename) : program(program), filename(filename) {}
    Type interp();
    Type interpModule();

    std::string entry;

    using compilerfunc_t = Type(*)(Interpreter&, const Loc&, const std::vector<Type>&);

    static std::unordered_map<std::string, compilerfunc_t> compilerFuncs;

    Scope* currentScope;
    std::string filename;
private:
    Type run(AST::Node node);
    Type run_list(AST::Node list);
    Type run_call(AST::Node call);
    Type run_funcDef(AST::Node func);
    Type run_varDef(AST::Node def);
    Type run_constDef(AST::Node def);
    Type run_nsDef(AST::Node def);
    Type run_podDef(AST::Node def);
    Type run_nsmemDec(AST::Node def);
    Type run_if(AST::Node def);
    Type run_while(AST::Node def);
    Type run_binOp(AST::Node op);
    Type run_identifier(AST::Node identifier);

    bool valueIsTrue(Type val);
    AST::Node program;
};


#endif //LANG_INTERPRETER_HPP
