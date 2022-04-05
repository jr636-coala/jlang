//
// Created by glover on 08/03/2022.
//

#ifndef LANG_INTERPRETER_HPP
#define LANG_INTERPRETER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "parser.hpp"
#include "typeval.hpp"

using Scope = TypeI<TypeT::ns>;

class Interpreter {
public:
    Interpreter(auto program, const auto& filename) : program(program), filename(filename) {}
    TypeVal interp();
    TypeVal interpModule();

    AST::Node entry;

    using compilerfunc_t = TypeVal(*)(Interpreter&, const Loc&, const std::vector<TypeVal>&);

    static std::unordered_map<std::string, compilerfunc_t> compilerFuncs;

    Scope* currentScope;
    std::string filename;
private:
    TypeVal run(AST::Node node);
    TypeVal run_list(AST::Node list);
    TypeVal run_call(AST::Node call);
    TypeVal run_funcDef(AST::Node func);
    TypeVal run_varDef(AST::Node def);
    TypeVal run_constDef(AST::Node def);
    TypeVal run_nsDef(AST::Node def);
    TypeVal run_podDef(AST::Node def);
    TypeVal run_if(AST::Node def);
    TypeVal run_while(AST::Node def);
    TypeVal run_binOp(AST::Node op);
    TypeVal run_identifier(AST::Node identifier);

    bool valueIsTrue(TypeVal val);
    AST::Node program;
};


#endif //LANG_INTERPRETER_HPP
