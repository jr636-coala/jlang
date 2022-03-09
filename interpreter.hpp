//
// Created by glover on 08/03/2022.
//

#ifndef LANG_INTERPRETER_HPP
#define LANG_INTERPRETER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "parser.hpp"

struct Scope;

struct Variable {
    Variable() : type(TYPE::UNKNOWN) {}
    Variable(const std::string& str) : type(TYPE::STRING), s(str) {}
    Variable(int x) : type(TYPE::INTEGER), i(x) {}
    Variable(AST::FunctionDefinition* func) : type(TYPE::FUNC), func(func) {}
    enum class TYPE { UNKNOWN, STRING, INTEGER, REF, FUNC } type;
    int i;
    std::string s;
    AST::FunctionDefinition* func;
    bool constant = false;
    Scope* ns {};
};

struct Scope {
    Scope* parentScope {nullptr};
    std::unordered_map<std::string, AST::FunctionDefinition*> functions;
    std::unordered_map<std::string, Variable> variables;

    std::optional<Variable> search(std::string str) {
        if (functions.contains(str)) return {Variable(functions[str])};
        if (variables.contains(str)) return {variables[str]};
        if (!parentScope) return {};
        return parentScope->search(str);
    }
};

class Interpreter {
public:
    Interpreter(auto* program) : program(program) {}
    void interp();

    std::string entry;

    using compilerfunc_t = void(*)(Interpreter&, const std::vector<Variable>&);

    static std::unordered_map<std::string, compilerfunc_t> compilerFuncs;
private:
    Variable run(AST::Node* node);
    Variable run(AST::StatementList* list);
    Variable run(AST::Call* call);
    Variable run(AST::FunctionDefinition* func);
    Variable run(AST::VariableDefinition* def);
    Variable run(AST::ConstDefinition* def);
    Variable run(AST::NamespaceDeclaration* def);
    Variable run(AST::BinaryOperator* op);
    Variable run(AST::Identifier* identifier);

    std::vector<Variable> resolveExpressionList(AST::ExpressionList* list);

    Scope* currentScope;
    AST::StatementList* program;
};


#endif //LANG_INTERPRETER_HPP
