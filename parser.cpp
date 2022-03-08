//
// Created by glover on 08/03/2022.
//

#include "parser.hpp"

using namespace AST;

auto Parser::parse() -> StatementList* {
    program = parse_statementList();
    return program;
}

StatementList* Parser::parse_statementList() {
    StatementList* node = new StatementList();
    node->statements.push_back(parse_expression());
    while(currentToken() == Token::semicolon) {
        eat(Token::semicolon);
        // Not sure if I should be doing this here
        if (currentToken() == Token::rcurly) break;
        node->statements.push_back(parse_expression());
    }
    return node;
}
StatementList* Parser::parse_statementList_prime() {
    eat(Token::lcurly);
    auto node = parse_statementList();
    eat(Token::rcurly);
    return node;
}

ExpressionList* Parser::parse_expressionList() {
    auto node = new ExpressionList();
    eat(Token::lparen);
    if (currentToken() == Token::rparen) return node; // No expressions in this list

    node->expressions.push_back(parse_expression()); // Add the first expression
    while(currentToken() != Token::rparen) { // Add the rest
        eat(Token::comma);
        node->expressions.push_back(parse_expression());
    }
    eat(Token::rparen);
    return node;
}

Expression* Parser::parse_expression_0() {
    switch (currentToken()) {
        case Token::plus: break; // unary plus
        case Token::minus: break; // unary minus
        case Token::number: break; // number literal
        case Token::string: { // string literal
            auto node = new String();
            node->string = eat(Token::string).match;
            return node;
        }
        case Token::lparen: { // nested expression
            eat(Token::lparen);
            auto exp = parse_expression();
            eat(Token::rparen);
            return exp;
        }
        case Token::identifier: {
            Identifier* identifier = new Identifier();
            identifier->identifier = eat(Token::identifier).match;
            switch (currentToken()) {
                case Token::lparen: return parse_call(identifier); // function call
                case Token::lsquare: return parse_index(identifier); // index
            }
            return identifier;
        }
        case Token::fn: return parse_functionDefinition(); // function definition
    }
    std::cout << "Error no expression\n";
}

Expression* Parser::parse_expression_1() {
    return parse_expression_0();
}
Expression* Parser::parse_expression_2() {
    return parse_expression_1();
}

Expression* Parser::parse_expression() {
    return parse_expression_2();
}

FunctionDefinition* Parser::parse_functionDefinition() {
    FunctionDefinition* node = new FunctionDefinition();
    eat(Token::fn);
    if (currentToken() == Token::identifier) {
        node->identifier = eat(Token::identifier).match;
    }
    eat(Token::lparen);
    node->parameters = parse_identifierList();
    eat(Token::rparen);
    node->body = parse_statementList_prime();
    return node;
}

std::vector<std::string> Parser::parse_identifierList() {
    std::vector<std::string> list;
    if (currentToken() == Token::identifier) {
        list.push_back(eat(Token::identifier).match);
        while(currentToken() == Token::comma) {
            eat(Token::comma);
            list.push_back(eat(Token::identifier).match);
        }
    }
    return list;
}

Call* Parser::parse_call(Expression* exp) {
    Call* node = new Call();
    node->expression = exp ? exp : parse_expression();
    node->arguments = parse_expressionList();
    return node;
}

Token Parser::currentToken() { return tokens[index].token; }

TokenInfo Parser::eat(Token tokenType) {
    if (index > tokens.size()) {
        throw;
    }
    const auto token = currentToken();
    if (token == tokenType) {
        return tokens[index++];
    }
    std::cout << index << " Could not eat " << token << " expecting " << tokenType << '\n';
    return { "", Token::null };
}

void Parser::printAST(Expression* node, int level) {
    for (auto i = 0; i < level; ++i) {
        std::cout << ' ';
    }

    std::cout << node->type << '\n';
    switch (node->type) {
        case NodeType::statementlist: {
            auto x = ((StatementList*)node)->statements;
            for (auto i = 0; i < x.size(); ++i) {
                printAST(x[i], level+1);
            }
        }
        break;
        case NodeType::expressionlist: {
            auto x = ((ExpressionList*)node)->expressions;
            for (auto i = 0; i < x.size(); ++i) {
                printAST(x[i], level+1);
            }
        }
        break;
        case NodeType::functiondefinition: {
            auto x = ((FunctionDefinition*)node);
            std::cout << "fn " << x->identifier << '\n';
            printAST(x->body);
        }
    }
}