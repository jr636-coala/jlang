//
// Created by glover on 08/03/2022.
//

#include "parser.hpp"
#include "log.hpp"
#include <numeric>

TokenInfo Parser::eat(Token tokenType) {
    if (index > tokens.size()) {
        throw;
    }
    const auto token = currentToken();
    if (token == tokenType) {
        return tokens[index++];
    }
    Log::error(std::string("Could not eat ") + token_to_string(token) + " expecting " + token_to_string(tokenType), tokens[index].loc);
    exit(0);
    //return { "", Token::null };
}

TokenInfo Parser::eat(std::vector<Token> tokenType) {
    if (index > tokens.size()) {
        throw;
    }
    const auto token = currentToken();
    for(auto& tokenType : tokenType) {
        if (token == tokenType) {
            return tokens[index++];
        }
    }
    auto tokenStr = std::accumulate(tokenType.begin() + 1, tokenType.end(), token_to_string(tokenType[0]),
                                    [](auto acc, const auto& x){ return std::move(acc) + " or " + token_to_string(x); });
    Log::error(std::string("Could not eat ") + token_to_string(token) + " expecting " + tokenStr, tokens[index].loc);
    exit(0);
}


using namespace AST;

auto Parser::parse() -> StatementList* {
    program = new StatementList();
    Statement* statement;
    while(index < tokens.size() && (statement = parse_statement())) {
        program->statements.push_back(statement);
        std::cout << index << '\n';
    }
    return program;
}

auto Parser::parseModule() -> AST::NamespaceDeclaration * {
    module = new NamespaceDeclaration();
    module->statements = new StatementList();
    Statement* statement;
    while(index < tokens.size() && (statement = parse_statement())) {
        module->statements->statements.push_back(statement);
        std::cout << index << '\n';
    }
    return module;
}

StatementList* Parser::parse_statementList() {
    eat(Token::lcurly);
    auto node = new StatementList();
    if (currentToken() == Token::rcurly) {
        eat(Token::rcurly);
        return node;
    }
    auto f_statement = parse_statement();
    node->statements.push_back(f_statement);
    Statement* statement;
    while(currentToken() != Token::rcurly && (statement = parse_statement())) {
        node->statements.push_back(statement);
    }
    eat(Token::rcurly);
    return node;
}

ExpressionList* Parser::parse_expressionList() {
    auto node = new ExpressionList();
    eat(Token::lparen);
    if (currentToken() != Token::rparen) { // Expressions in this list

        node->expressions.push_back(parse_expression()); // Add the first expression
        while (currentToken() != Token::rparen) { // Add the rest
            eat(Token::comma);
            node->expressions.push_back(parse_expression());
        }
    }
    eat(Token::rparen);
    return node;
}

Statement* Parser::parse_statement() {
    switch (currentToken()) {
        case Token::semicolon: {
            eat(Token::semicolon);
            return parse_statement();
        }
        case Token::dcolon: {
            eat(Token::dcolon);
            auto node = new NSMemberDeclaration();
            node->identifier = eat(Token::identifier).match;
            eat(Token::assign);
            node->expression = parse_expression();
            return node;
        }
        case Token::ret: {
            eat(Token::ret);
            return new ReturnStatement(parse_expression());
        }
        case Token::tif: {
            eat(Token::tif);
            eat(Token::lparen);
            auto node = new ConditionalStatement();
            node->condition = parse_expression();
            eat(Token::rparen);
            if (currentToken() == Token::lcurly) {
                node->_true = parse_statementList();
            }
            else {
                node->_true = parse_statement();
            }
            if (currentToken() == Token::telse) {
                eat(Token::telse);
                if (currentToken() == Token::lcurly) {
                    node->_else = parse_statementList();
                }
                else {
                    node->_else = parse_statement();
                }
            }
            return node;
        }
        case Token::twhile: {
            eat(Token::twhile);
            eat(Token::lparen);
            auto node = new WhileStatement();
            node->condition = parse_expression();
            eat(Token::rparen);
            if (currentToken() == Token::lcurly) {
                node->body = parse_statementList();
            }
            else {
                node->body = parse_statement();
            }
            return node;
        }
        case Token::pod: {
            eat(Token::pod);
            auto node = new PodDefinition();
            node->name = eat(Token::identifier).match;
            node->body = parse_statementList();
            return node;
        }
    }

    return parse_expression();
}

Expression* Parser::parse_identifier() {
    auto identifier = new Identifier();
    identifier->identifier = eat(Token::identifier).match;
    while (currentToken() == Token::dcolon) {
        eat(Token::dcolon);
        identifier->identifier += "::" + eat(Token::identifier).match;
    }
    switch (currentToken()) {
        case Token::lparen: return parse_call(identifier); // function call
        case Token::lsquare: return parse_index(identifier); // index
    }
    return identifier;
}

Expression* Parser::parse_expression_0() {
    switch (currentToken()) {
        case Token::plus: break; // unary plus
        case Token::minus: break; // unary minus
        case Token::number: return new Number(eat(Token::number).match); // number literal
        case Token::string: return new String(eat(Token::string).match); // string literal
        case Token::lparen: { // nested expression
            eat(Token::lparen);
            auto exp = parse_expression();
            eat(Token::rparen);
            return exp;
        }
        case Token::identifier: return parse_identifier();
        case Token::fn: return parse_functionDefinition(); // function definition
        case Token::let: return parse_variableDefinition(); // variable definition
        case Token::tconst: return parse_constDefinition(); // constant definition
        case Token::dcolon: return parse_namespace(); // namespace definition
    }
    Log::warning("No expression", currentToken().loc);
    return nullptr;
}

Expression* Parser::parse_expression_1() {
    auto l = parse_expression_0();
    switch (currentToken()) {
        case Token::star: return new BinaryOperator(l, eat(Token::star), parse_expression_1());
        case Token::slash: return new BinaryOperator(l, eat(Token::slash), parse_expression_1());
    }
    return l;
}
Expression* Parser::parse_expression_2() {
    auto l = parse_expression_1();
    switch (currentToken()) {
        case Token::plus: return new BinaryOperator(l, eat(Token::plus), parse_expression_2());
        case Token::minus: return new BinaryOperator(l, eat(Token::minus), parse_expression_2());
    }
    return l;
}

AST::Expression* Parser::parse_expression_3() {
    auto l = parse_expression_2();
    switch (currentToken()) {
        case Token::assign: return new BinaryOperator(l, eat(Token::assign), parse_expression_3());
        case Token::plusequal: return new BinaryOperator(l, eat(Token::plusequal), parse_expression_3());
        case Token::minusequal: return new BinaryOperator(l, eat(Token::minusequal), parse_expression_3());
    }
    return l;
}
AST::Expression* Parser::parse_expression_6() {}
AST::Expression* Parser::parse_expression_5() {}
AST::Expression* Parser::parse_expression_7() {}
AST::Expression* Parser::parse_expression_4() {}
AST::Expression* Parser::parse_expression_8() {}
AST::Expression* Parser::parse_expression_9() {}

Expression* Parser::parse_expression() {
    return parse_expression_3();
}

FunctionDefinition* Parser::parse_functionDefinition() {
    auto node = new FunctionDefinition();
    eat(Token::fn);
    if (currentToken() == Token::identifier) {
        node->identifier = eat(Token::identifier).match;
    }
    eat(Token::lparen);
    node->parameters = parse_identifierList();
    eat(Token::rparen);
    node->body = parse_statementList();
    return node;
}

std::vector<std::pair<Token, std::string>> Parser::parse_identifierList() {
    std::vector<std::pair<Token, std::string>> list;
    const auto nameTypePair = [this]() -> std::pair<Token, std::string> {
        auto id = eat(Token::identifier).match;
        if (currentToken() == Token::colon) {
            eat(Token::colon);
            return {eat(TypeTokens), id};
        }
        return {};
    };
    if (currentToken() == Token::identifier) {
        list.push_back(nameTypePair());
        while(currentToken() == Token::comma) {
            eat(Token::comma);
            list.push_back(nameTypePair());
        }
    }
    return list;
}

Call* Parser::parse_call(Expression* exp) {
    auto node = new Call();
    node->loc = loc();
    node->expression = exp ? exp : parse_expression();
    node->arguments = parse_expressionList();
    return node;
}

TokenInfo Parser::currentToken() { return index < tokens.size() ? tokens[index] : TokenInfo{}; }
Loc Parser::loc() { return currentToken().loc; };

void Parser::printAST(Node* node, int level) {
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

AST::VariableDefinition* Parser::parse_variableDefinition() {
    auto node = new VariableDefinition();
    eat(Token::let);
    node->identifier = eat(Token::identifier).match;
    if (currentToken() == Token::colon) {
        eat(Token::colon);
        node->type = new std::string(eat(Token::identifier).match);
    }
    if (currentToken() == Token::assign) {
        eat(Token::assign);
        node->expression = parse_expression();
    }
    return node;
}

AST::ConstDefinition* Parser::parse_constDefinition() {
    auto node = new ConstDefinition();
    eat(Token::tconst);
    node->identifier = eat(Token::identifier).match;
    if (currentToken() == Token::colon) {
        eat(Token::colon);
        node->type = new std::string(eat(Token::identifier).match);
    }
    eat(Token::assign);
    if (currentToken() == Token::dcolon) node->expression = parse_namespace();
    else node->expression = parse_expression();
    return node;
}

AST::NamespaceDeclaration* Parser::parse_namespace() {
    eat(Token::dcolon);
    auto node = new NamespaceDeclaration();
    node->statements = parse_statementList();
    return node;
}
