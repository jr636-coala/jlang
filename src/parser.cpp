//
// Created by glover on 08/03/2022.
//

#include "parser.hpp"
#include "log.hpp"
#include <numeric>

using namespace AST;

TokenInfo Parser::eat(Token tokenType) {
    if (_index > tokens.size()) {
        throw;
    }
    const auto token = currentToken();
    if (token == tokenType) {
        return tokens[_index++];
    }
    Log::error(std::string("Could not eat ") + token_to_string(token) + " expecting " + token_to_string(tokenType), tokens[_index].loc);
    exit(0);
    //return { "", Token::null };
}

TokenInfo Parser::eat(std::vector<Token> tokenType) {
    if (_index > tokens.size()) {
        throw;
    }
    const auto token = currentToken();
    for(auto& tokenType : tokenType) {
        if (token == tokenType) {
            return tokens[_index++];
        }
    }
    auto tokenStr = std::accumulate(tokenType.begin() + 1, tokenType.end(), token_to_string(tokenType[0]),
                                    [](auto acc, const auto& x){ return std::move(acc) + " or " + token_to_string(x); });
    Log::error(std::string("Could not eat ") + token_to_string(token) + " expecting " + tokenStr, tokens[_index].loc);
    exit(0);
}

auto Parser::parse() -> Node {
    program = Node(Node::Type::list);
    while(_index < tokens.size()) {
        Node stmt = statement();
        if (stmt.type == Node::Type::null) break;
        program.params.push_back(stmt);
        std::cout << _index << '\n';
    }
    return program;
}

auto Parser::parseModule() -> Node {
    module = Node(Node::Type::ns);
    while(_index < tokens.size()) {
        Node stmt = statement();
        if (stmt.type == Node::Type::null) break;
        module.params.push_back(stmt);
        std::cout << _index << '\n';
    }
    return module;
}

Node Parser::statements() {
    eat(Token::lcurly);
    Node node(Node::Type::list);
    if (currentToken() == Token::rcurly) {
        eat(Token::rcurly);
        return node;
    }
    auto f_statement = statement();
    node.params.push_back(f_statement);
    while(currentToken() != Token::rcurly) {
        Node stmt = statement();
        if (stmt.type == Node::Type::null) break;
        node.params.push_back(stmt);
    }
    eat(Token::rcurly);
    return node;
}

Node Parser::statement_or_body() {
    if (currentToken() == Token::lcurly) {
        return statements();
    }
    return statement();
}

Node Parser::condition() {
    eat(Token::lparen);
    Node node = expression();
    eat(Token::rparen);
    return node;
}

Node Parser::statement() {
    switch (currentToken()) {
        case Token::fn: return func_def();
        /*case Token::pod: {
            eat(Token::pod);
            Node node(Node::Type::pod_def);
            node.identifier.name = eat(Token::identifier).match;
            node.params.push_back(statements());
            return node;
        }*/
        case Token::pod: return pod_def();
        case Token::tif: {
            eat(Token::tif);
            Node node(Node::Type::iff);
            node.params.push_back(condition());
            node.params.push_back(statement_or_body());
            if (currentToken() == Token::telse) {
                eat(Token::telse);
                node.params.push_back(statement_or_body());
            }
            return node;
        }
        case Token::twhile: {
            eat(Token::twhile);
            eat(Token::lparen);
            Node node(Node::Type::whilee);
            node.params.push_back(expression());
            eat(Token::rparen);
            if (currentToken() == Token::lcurly) {
                node.params.push_back(statements());
            }
            else {
                node.params.push_back(statement());
            }
            return node;
        }
        case Token::ret: {
            eat(Token::ret);
            Node node(Node::Type::returnn);
            node.params.push_back(expression());
            eat(Token::semicolon);
            return node;
        }
        case Token::let: {
            eat(Token::let);
            Node node(Node::Type::let);
            node.identifier = typed_identifier();
            if (currentToken() == Token::assign) {
                eat(Token::assign);
                node.params.push_back(expr_or_list());
            }
            eat(Token::semicolon);
            return node;
        }
        case Token::tconst: {
            eat(Token::tconst);
            Node node(Node::Type::constt);
            node.identifier = typed_identifier();
            eat(Token::assign);
            node.params.push_back(expr_or_list());
            eat(Token::semicolon);
            return node;
        }
    }
    Node node = expression();
    eat(Token::semicolon);
    return node;
}

Node Parser::parse_identifier() {
    Node identifier(Node::Type::identifier);
    identifier.identifier.name = eat(Token::identifier).match;
    while (currentToken() == Token::dcolon) {
        eat(Token::dcolon);
        identifier.identifier.name += "::" + eat(Token::identifier).match;
    }
    switch (currentToken()) {
        case Token::lparen: return parse_call(identifier.identifier); // function call
        case Token::lsquare: return index(identifier.identifier); // _index
    }
    return identifier;
}

Node Parser::parse_expression_0() {
    switch (currentToken()) {
        case Token::number: return Node::Number(eat(Token::number).match); // number literal
        case Token::string: return Node::String(eat(Token::string).match); // string literal
        case Token::lparen: { // nested expression
            eat(Token::lparen);
            auto exp = expression();
            eat(Token::rparen);
            return exp;
        }
        case Token::identifier: return parse_identifier();
        case Token::fn: return func_def();
        case Token::dcolon: {
            eat(Token::dcolon);
            Node node(Node::Type::ns);
            node.params = statements().params;
            return node;
        }
    }
    Log::warning("No expression", currentToken().loc);
    return Node();
}

Node Parser::parse_expression_1() {
    switch (currentToken()) {
        case Token::star: {
            eat(Token::star);
            Node node(Node::Type::deref);
            node.params.push_back(parse_expression_0());
            return node;
        }
        case Token::band: {
            eat(Token::band);
            Node node(Node::Type::addr);
            node.params.push_back(parse_expression_0());
            return node;
        }
        case Token::plus: {
            eat(Token::plus);
            Node node(Node::Type::uplus);
            node.params.push_back(parse_expression_0());
            return node;
        }
        case Token::minus: {
            eat(Token::minus);
            Node node(Node::Type::uminus);
            node.params.push_back(parse_expression_0());
            return node;
        }
    }
    return parse_expression_0();
}
Node Parser::parse_expression_2() {
    auto l = parse_expression_1();
    switch (currentToken()) {
        case Token::star:
        case Token::slash: {
            auto token = eat(currentToken());
            return Node::BinOp(l, token, parse_expression_2());
        }
    }
    return l;
}
Node Parser::parse_expression_3() {
    auto l = parse_expression_2();
    switch (currentToken()) {
        case Token::plus: 
        case Token::minus:
        case Token::perc: {
            auto token = eat(currentToken());
            return Node::BinOp(l, token, parse_expression_3());
        }
    }
    return l;
}
Node Parser::parse_expression_4() {
    auto l = parse_expression_3();
    switch (currentToken()) {
        case Token::andd: {
            auto token = eat(currentToken());
            return Node::BinOp(l, token, parse_expression_4());
        }
    }
    return l;
}
Node Parser::parse_expression_5() {
    auto l = parse_expression_4();
    switch (currentToken()) {
        case Token::orr: {
            auto token = eat(currentToken());
            return Node::BinOp(l, token, parse_expression_5());
        }
    }
    return l;
}
Node Parser::parse_expression_6() {
    auto l = parse_expression_5();
    switch (currentToken()) {
        case Token::assign:
        case Token::plusequal:
        case Token::minusequal: {
            auto token = eat(currentToken());
            return Node::BinOp(l, token, parse_expression_6());
        }
    }
    return l;
}
Node Parser::parse_expression_7() {}
Node Parser::parse_expression_8() {}
Node Parser::parse_expression_9() {}

Node Parser::expression() {
    return parse_expression_6();
}

Node Parser::func_def() {
    auto node = Node::FuncDef();
    eat(Token::fn);
    node.identifier = optional_identifier();
    eat(Token::lparen);
    node.params[0] = param_list();
    eat(Token::rparen);
    node.params[1] = statements();
    return node;
}

Identifier Parser::optional_identifier() {
    if (currentToken() == Token::identifier) {
        return IDENTIFIER();
    }
    return {};
}

Node Parser::pod_def() {
    eat(Token::pod);
    Node node(Node::Type::pod_def);
    node.identifier = IDENTIFIER();
    node.params.push_back(statements());
    return node;
}

Identifier Parser::IDENTIFIER() {
    Identifier identifier;
    identifier.name = eat(Token::identifier).match;
    return identifier;
}

Node Parser::param_list() {
    Node node(Node::Type::list);
    const auto nameTypePair = [this]() -> Node {
        auto id = eat(Token::identifier).match;
        Node node(Node::Type::identifier);
        node.identifier.name = id;
        if (currentToken() == Token::colon) {
            eat(Token::colon);
            node.valType.type = token_to_type(eat(TypeTokens));
        }
        return node;
    };
    if (currentToken() == Token::identifier) {
        node.params.push_back(nameTypePair());
        while(currentToken() == Token::comma) {
            eat(Token::comma);
            node.params.push_back(nameTypePair());
        }
    }
    return node;
}

Identifier Parser::typed_identifier() {
    Identifier identifier = IDENTIFIER();
    if (currentToken() == Token::colon) {
        eat(Token::colon);
        if (currentToken() == Token::identifier) {
            identifier.valType = {TypeT::pod, IDENTIFIER().name};
        }
        else {
            identifier.valType = {token_to_type(eat(TypeTokens)), ""};
        }
    }
    return identifier;
}

Node Parser::expr_or_list() {
    if (currentToken() == Token::lcurly) {
        eat(Token::lcurly);
        Node node = expression_list();
        eat(Token::rcurly);
        return node;
    }
    return expression();
}

Node Parser::expression_list() {
    Node node(Node::Type::list);
    Node expr(Node::Type::list);
    while((expr = expression()).type != Node::Type::null) {
        node.params.push_back(expr);
    }
    return node;
}

Node Parser::index(Identifier identifier) {
    Node node(Node::Type::index);
    node.identifier = identifier;
    while(currentToken() == Token::lsquare) {
        eat(Token::lsquare);
        node.params.push_back(expression());
        eat(Token::rsquare);
    }

    return node;
}

Node Parser::parse_call(Identifier identifier) {
    Node node(Node::Type::call);
    node.identifier = identifier;
    node.loc = loc();
    node.params.push_back(expression_list());
    return node;
}

TokenInfo Parser::currentToken() { return _index < tokens.size() ? tokens[_index] : TokenInfo{}; }
Loc Parser::loc() { return currentToken().loc; };

void Parser::printAST(Node node, int level) {
    for (auto i = 0; i < level; ++i) {
        std::cout << ' ';
    }

    std::cout << node.type << '\n';
    switch (node.type) {
        case Node::Type::list: {
            for (auto i = 0; i < node.params.size(); ++i) {
                printAST(node.params[i], level+1);
            }
        }
        break;
        break;
        case Node::Type::func_def: {
            std::cout << "fn " << node.identifier.name << '\n';
            printAST(node.params[1]);
        }
    }
}