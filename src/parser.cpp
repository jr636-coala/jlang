//
// Created by glover on 08/03/2022.
//

#include "parser.hpp"
#include "log.hpp"
#include <numeric>

using namespace AST;

TokenInfo Parser::eat(Token tokenType) {
    if (_index > tokens.size()) throw;
    const auto token = currentToken();
    if (token == tokenType) return tokens[_index++];
    Log::error(std::string("Could not eat ") + token_to_string(token) + " expecting " + token_to_string(tokenType), tokens[_index].loc);
    exit(0);
    //return { "", Token::null };
}

TokenInfo Parser::eat(std::vector<Token> tokenType) {
    if (_index > tokens.size()) throw;
    const auto token = currentToken();
    for(auto& tokenType : tokenType) {
        if (token == tokenType) return tokens[_index++];
    }
    auto tokenStr = std::accumulate(tokenType.begin() + 1, tokenType.end(), token_to_string(tokenType[0]),
                                    [](auto acc, const auto& x){ return std::move(acc) + " or " + token_to_string(x); });
    Log::error(std::string("Could not eat ") + token_to_string(token) + " expecting " + tokenStr, tokens[_index].loc);
    exit(0);
}

Node Parser::parse() {
    program = Node(Node::Type::list);
    while(_index < tokens.size()) {
        Node stmt = statement();
        if (stmt.type == Node::Type::null) continue;;
        program.params.push_back(stmt);
    }
    return program;
}

Node Parser::parseModule() {
    module = Node(Node::Type::ns);
    while(_index < tokens.size()) {
        Node stmt = statement();
        if (stmt.type == Node::Type::null) continue;
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
    while(currentToken() != Token::rcurly) {
        Node stmt = statement();
        if (stmt.type == Node::Type::null) continue;
        node.params.push_back(stmt);
    }
    eat(Token::rcurly);
    return node;
}

Node Parser::statement_or_body() {
    if (currentToken() == Token::lcurly) return statements();
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
    identifier.identifier = IDENTIFIER();
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
    //Log::warning("No expression", currentToken().loc);
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

Node Parser::expression() { return parse_expression_6(); }

Node Parser::func_def() {
    auto node = Node::FuncDef();
    eat(Token::fn);
    node.identifier = optional_identifier();
    eat(Token::lparen);
    node.params[0] = param_list();
    eat(Token::rparen);
    if (currentToken() == Token::colon) {
        eat(Token::colon);
        node.valType = type_mod();
    }
    node.params[1] = statements();
    return node;
}

Identifier Parser::optional_identifier() {
    if (currentToken() == Token::identifier) return IDENTIFIER();
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
        Node node(Node::Type::identifier);
        node.identifier = typed_identifier();
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
        identifier.type = type_mod();
    }
    return identifier;
}

Type Parser::type_mod() {
    Type type;
    if (currentToken() == Token::identifier) {
        type.type = TypeT::pod;
        type.name = IDENTIFIER().name;
    }
    else type.type = token_to_type(eat(currentToken()));
    type.modifier = type_mod_impl();
    return type;
}

std::vector<TypeT> Parser::type_mod_impl() {
    std::vector<TypeT> out;
    top:
    switch (currentToken()) {
        case Token::band: out.push_back(TypeT::reference); goto top;
        case Token::star: out.push_back(TypeT::pointer); goto top;
    }
    return out;
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
    while((expr = expression()).type != Node::Type::null) node.params.push_back(expr);
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
    const auto printLevel = [level](int off = 0){ for (auto i = 0; i < level + off; ++i) std::cout << ' '; };

    const auto& type = node.type;
    const auto& params = node.params;

    switch (type) {
        case Node::Type::null: std::cout << "###NULL###"; break;
        case Node::Type::plus:
        case Node::Type::minus:
        case Node::Type::assign:
        case Node::Type::plusassign:
        case Node::Type::minusassign:
        case Node::Type::equal:
        case Node::Type::slash:
        case Node::Type::star:
        case Node::Type::perc:
        case Node::Type::andd:
        case Node::Type::orr: {
            printAST(params[0], level);
            std::cout << ' ' << type << ' ';
            printAST(params[1], level);
            break;
        }
        case Node::Type::uplus:
        case Node::Type::uminus:
        case Node::Type::deref:
        case Node::Type::addr: {
            std::cout << type;
            printAST(params[0], level);
            break;
        }
        case Node::Type::pod_access: std::cout << "UNHANDLED PRINT"; break;
        case Node::Type::call: {
            std::cout << node.identifier << '(';
            if (params[0].params.size()) printAST(params[0].params[0], level);
            for (auto i = 1; i < params[0].params.size(); ++i) {
                std::cout << ", ";
                printAST(params[0].params[i], level);
            }
            std::cout << ')';
            break;
        }
        case Node::Type::index: std::cout << "UNHANDLED PRINT"; break;
        case Node::Type::ns: std::cout << "UNHANDLED PRINT"; break;
        case Node::Type::number: {
            std::cout << node.value;
            break;
        }
        case Node::Type::string: {
            std::cout << '"' << node.value << '"';
            break;
        }
        case Node::Type::identifier: {
            std::cout << node.identifier;
            break;
        }

        case Node::Type::func_def: {
            std::cout << "fn " << node.identifier << '(';
            if (params[0].params.size()) printAST(params[0].params[0], level);
            for (auto i = 1; i < params[0].params.size(); ++i) {
                std::cout << ", ";
                printAST(params[0].params[i], level);
            }
            std::cout << "): ";
            std::cout << node.valType;
            std::cout << "{\n";
            for (auto i = 0; i < params[1].params.size(); ++i) {
                printLevel(1);
                printAST(params[1].params[i], level + 1);
                std::cout << ";\n";
            }
            printLevel();
            std::cout << '}';
            break;
        }
        case Node::Type::pod_def: {
            std::cout << "pod " << node.identifier << " {\n";
            printAST(params[0], level + 1);
            printLevel();
            std::cout << '}';
            break;
        }
        case Node::Type::iff: {
            std::cout << "if(";
            printAST(params[0], level);
            std::cout << ") ";
            if (params[1].type == Node::Type::list) {
                std::cout << "{\n";
                printAST(params[1], level + 1);
                printLevel();
                std::cout << '}';
            }
            else printAST(params[1], level);

            if (params.size() > 2) {
                if (params[2].type == Node::Type::list) {
                    std::cout << "else {\n";
                    printAST(params[2], level + 1);
                    printLevel();
                    std::cout << '}';
                }
                else printAST(params[2], level);
            }
            break;
        }
        case Node::Type::whilee: {
            std::cout << "while(";
            printAST(params[0], level);
            std::cout << ") ";
            if (params[1].type == Node::Type::list) {
                std::cout << "{\n";
                printAST(params[1], level + 1);
                printLevel();
                std::cout << '}';
            }
            else printAST(params[1], level);
            break;
        }
        case Node::Type::returnn: {
            std::cout << "return ";
            printAST(params[0], level);
            break;
        }
        case Node::Type::let:
        case Node::Type::constt: {
            std::cout << type << ' ' << node.identifier;
            if (node.params.size()) {
                std::cout << " = ";
                printAST(node.params[0], level);
            }
            break;
        }
        case Node::Type::list: {
            for(auto i = 0; i < params.size(); ++i) {
                printLevel();
                printAST(params[i], level);
                std::cout << '\n';
            }
            break;
        }
    }
}