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

    AST::Node parse();
    AST::Node parseModule();

    static void printAST(AST::Node, int level = 0);

private:
    AST::Node statements();

    AST::Node statement();
    AST::Node condition();
    AST::Node statement_or_body();
    AST::Node func_def();
    AST::Identifier optional_identifier();
    AST::Node pod_def();
    AST::Node param_list(); // FIXME
    AST::Identifier typed_identifier();
    AST::Type type_mod();
    std::vector<TypeT> type_mod_impl();
    AST::Node expression();
    AST::Node expr_or_list();
    AST::Node expression_list();
    AST::Node pod_access();
    AST::Node index(AST::Identifier);

    AST::Node parse_expression_0();
    AST::Node parse_expression_1();
    AST::Node parse_expression_2();
    AST::Node parse_expression_3();
    AST::Node parse_expression_4();
    AST::Node parse_expression_5();
    AST::Node parse_expression_6();
    AST::Node parse_expression_7();
    AST::Node parse_expression_8();
    AST::Node parse_expression_9();

    AST::Node parse_namespace();

    AST::Node parse_identifier();

    AST::Identifier IDENTIFIER();

    AST::Node parse_call(AST::Identifier);
    AST::Node parse_index(auto) { return AST::Node(); }
    TokenInfo currentToken();
    Loc loc();

    TokenInfo eat(Token tokenType);
    TokenInfo eat(std::vector<Token> tokenType);

    long _index {0};
    std::vector<TokenInfo> tokens;
    AST::Node program;
    AST::Node module;
};


#endif //LANG_PARSER_HPP
