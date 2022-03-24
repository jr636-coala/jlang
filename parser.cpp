//
// Created by glover on 08/03/2022.
//

#include "parser.hpp"
#include "log.hpp"

using namespace AST;

auto Parser::parse() -> StatementList* {
    program = parse_statementList();
    return program;
}

StatementList* Parser::parse_statementList() {
    StatementList* node = new StatementList();
    auto f_statement = parse_statement();
    if (f_statement == nullptr) return node;
    node->statements.push_back(f_statement);
    while(currentToken() == Token::semicolon) {
        eat(Token::semicolon);
        // Not sure if I should be doing this here
        if (currentToken() == Token::rcurly || currentToken() == Token::null) break;
        node->statements.push_back(parse_statement());
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
        case Token::dcolon: {
            eat(Token::dcolon);
            auto* node = new NSMemberDeclaration();
            node->identifier = eat(Token::identifier).match;
            eat(Token::assign);
            node->expression = parse_expression();
            return node;
        }
    }

    return parse_expression();
}

Expression* Parser::parse_identifier() {
    Identifier* identifier = new Identifier();
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
        case Token::number: { // number literal
            auto node = new Number();
            node->number = eat(Token::number).match;
            return node;
        }
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
        case Token::star: {
            eat(Token::star);
            auto node = new BinaryOperator();
            node->l = l;
            node->op = Token::star;
            node->r = parse_expression_1();
            return node;
        }
    }
    return l;
}
Expression* Parser::parse_expression_2() {
    auto l = parse_expression_1();
    switch (currentToken()) {
        case Token::plus: {
            auto node = new BinaryOperator();
            node->l = l;
            node->op = eat(Token::plus);
            node->r = parse_expression_2();
            return node;
        }
    }
    return l;
}

AST::Expression* Parser::parse_expression_3() {
    auto l = parse_expression_2();
    switch (currentToken()) {
        case Token::assign: {
            auto node = new BinaryOperator();
            node->l = l;
            node->op = eat(Token::assign);
            node->r = parse_expression_3();
            return node;
        }
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

TokenInfo Parser::currentToken() { return index < tokens.size() ? tokens[index] : TokenInfo{}; }

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
        node->type = new Type(eat(Token::identifier).match);
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
        node->type = new Type(eat(Token::identifier).match);
    }
    eat(Token::assign);
    if (currentToken() == Token::dcolon) node->expression = parse_namespace();
    else node->expression = parse_expression();
    return node;
}

AST::NamespaceDeclaration* Parser::parse_namespace() {
    eat(Token::dcolon);
    auto node = new NamespaceDeclaration();
    node->statements = parse_statementList_prime();
    return node;
}
