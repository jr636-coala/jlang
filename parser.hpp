//
// Created by glover on 08/03/2022.
//

#ifndef LANG_PARSER_HPP
#define LANG_PARSER_HPP

#include "astnode.hpp"
#include "tokeniser.hpp"

class Parser {
public:
    Parser(auto& tokens) : tokens(tokens) {}

    auto parse() -> AST::StatementList*;

    static void printAST(AST::Node*, int level = 0);

private:
    AST::StatementList* parse_statementList();
    auto parse_statementList_prime() -> AST::StatementList*;

    auto parse_expressionList() -> AST::ExpressionList*;

    AST::Statement* parse_statement();

    AST::Expression* parse_expression_0();
    AST::Expression* parse_expression_1();
    AST::Expression* parse_expression_2();
    AST::Expression* parse_expression();

    AST::FunctionDefinition* parse_functionDefinition();
    AST::VariableDefinition* parse_variableDefinition();
    AST::ConstDefinition* parse_constDefinition();
    AST::NamespaceDeclaration* parse_namespace();

    std::vector<std::string> parse_identifierList();

    AST::Call* parse_call(AST::Expression* exp = nullptr);
    AST::Expression* parse_index(auto) { return nullptr; }
    TokenInfo currentToken();
    TokenInfo eat(Token tokenType);


    long index {0};
    std::vector<TokenInfo> tokens;
    AST::StatementList* program;
};


#endif //LANG_PARSER_HPP
