//
// Created by glover on 08/03/2022.
//

#ifndef LANG_ASTNODE_HPP
#define LANG_ASTNODE_HPP
#pragma once
#include <string>
#include <ostream>
#include <vector>

#define AST_NODE_TYPES(_) \
  _(null)                        \
  _(statementlist) \
  _(expressionlist) \
  _(call) \
  _(functiondefinition) \
  _(string)               \
  _(identifier)           \
  _(index)     \

namespace AST {
    enum class NodeType {
#define AST_NODE_FUNCTION(T) T,
        AST_NODE_TYPES(AST_NODE_FUNCTION)
    };

    struct Node {
        Node(NodeType type): type(type) {}
        NodeType type;
    };

    struct Expression {
        Expression(NodeType type): type(type) {}
        NodeType type;
    };

    struct StatementList : Expression {
        StatementList(): Expression(NodeType::statementlist) {}
        std::vector<Expression*> statements;
    };

    struct ExpressionList : Expression {
        ExpressionList(): Expression(NodeType::expressionlist) {}
        std::vector<Expression*> expressions;
    };

    struct Call : Expression {
        Call(): Expression(NodeType::call) {}
        Expression* expression;
        ExpressionList* arguments;
    };

    struct Index : Expression {
        Index(): Expression(NodeType::index) {}
        Expression* expression;
        ExpressionList* arguments;
    };

    struct FunctionDefinition : Expression {
        FunctionDefinition(): Expression(NodeType::functiondefinition) {}
        std::string funcType; // This will be changed
        std::string identifier;
        std::vector<std::string> parameters;
        StatementList* body;
    };

    struct String : Expression {
        String() : Expression(NodeType::string) {}
        std::string string;
    };

    struct Identifier : Expression {
        Identifier() : Expression(NodeType::identifier) {}
        std::string identifier;
    };
}

inline std::ostream& operator<<(std::ostream& os, AST::NodeType nodeType) {
    switch (nodeType) {
#define AST_NODE_FUNCTION(T) case AST::NodeType::T: os << #T; break;
        AST_NODE_TYPES(AST_NODE_FUNCTION)
    }
    return os;
}

#endif //LANG_ASTNODE_HPP
