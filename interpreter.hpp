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
    Interpreter(auto* program, const auto& filename) : program(program), filename(filename) {}
    Type interp();
    Type interpModule();

    std::string entry;

    using compilerfunc_t = Type(*)(Interpreter&, const Loc&, const std::vector<Type>&);

    static std::unordered_map<std::string, compilerfunc_t> compilerFuncs;

    Scope* currentScope;
    std::string filename;
private:
    Type run(AST::Node* node);
    Type run(AST::StatementList* list);
    Type run(AST::Call* call);
    Type run(AST::FunctionDefinition* func);
    Type run(AST::VariableDefinition* def);
    Type run(AST::ConstDefinition* def);
    Type run(AST::NamespaceDeclaration* def);
    Type run(AST::PodDefinition* def);
    Type run(AST::NSMemberDeclaration* def);
    Type run(AST::ConditionalStatement* def);
    Type run(AST::WhileStatement* def);
    Type run(AST::BinaryOperator* op);
    Type run(AST::Identifier* identifier);

    std::vector<Type> resolveExpressionList(AST::ExpressionList* list);

    bool valueIsTrue(Type val);
    AST::Node* program;
};


#endif //LANG_INTERPRETER_HPP
