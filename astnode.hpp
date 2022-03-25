//
// Created by glover on 08/03/2022.
//

#ifndef LANG_ASTNODE_HPP
#define LANG_ASTNODE_HPP

#include <string>
#include <ostream>
#include <vector>
#include "loc.hpp"
#include "tokeniser.hpp"

#define AST_NODE_TYPES(_) \
  _(null)                        \
  _(statementlist) \
  _(expressionlist) \
  _(call) \
  _(functiondefinition) \
  _(variabledefinition)   \
  _(constdefintiion)      \
  _(namespacedeclaration) \
  _(nsmemberdeclaration) \
  _(binaryoperator)\
  _(number)                        \
  _(string)               \
  _(identifier)           \
  _(index)

namespace AST {
    enum class NodeType {
#define AST_NODE_FUNCTION(T) T,
        AST_NODE_TYPES(AST_NODE_FUNCTION)
    };

    struct Node {
        Node(NodeType type): type(type) {}
        NodeType type;
        Loc loc;
    };

    struct Statement : Node {
        Statement(NodeType type): Node(type) {}
    };

    struct Expression : Statement {
        Expression(NodeType type): Statement(type) {}
    };

    struct BinaryOperator : Expression {
        BinaryOperator(): Expression(NodeType::binaryoperator) {}
        Expression* l;
        Token op;
        Expression* r;
    };

    struct StatementList : Statement {
        StatementList(): Statement(NodeType::statementlist) {}
        std::vector<Statement*> statements;
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

    struct Identifier : Expression {
        Identifier() : Expression(NodeType::identifier) {}
        std::string identifier;
    };

    struct FunctionDefinition : Expression {
        FunctionDefinition(): Expression(NodeType::functiondefinition) {}
        std::string funcType; // This will be changed
        std::string identifier;
        std::vector<std::string> parameters;
        StatementList* body;
    };

    struct VariableDefinition : Expression {
        VariableDefinition() : Expression(NodeType::variabledefinition) {}
        std::string identifier;
        Expression* expression;
        std::string* type;
    };

    struct ConstDefinition : Expression {
        ConstDefinition() : Expression(NodeType::constdefintiion) {}
        std::string identifier;
        Expression* expression;
        std::string* type;
    };

    struct NamespaceDeclaration : Expression {
        NamespaceDeclaration() : Expression(NodeType::namespacedeclaration) {}
        StatementList* statements;
    };

    struct NSMemberDeclaration : Statement {
        NSMemberDeclaration() : Statement(NodeType::nsmemberdeclaration) {}
        std::string identifier;
        Expression* expression;
    };

    struct String : Expression {
        String() : Expression(NodeType::string) {}
        std::string string;
    };

    struct Number : Expression {
        Number() : Expression(NodeType::number) {}
        std::string number;
    };
}

inline std::string node_type_to_string(AST::NodeType nodeType) {
    switch (nodeType) {
#define AST_NODE_FUNCTION(T) case AST::NodeType::T: return #T;
        AST_NODE_TYPES(AST_NODE_FUNCTION)
        default: return "# UNKNOWN NODE TYPE #";
    }
}

inline std::ostream& operator<<(std::ostream& os, AST::NodeType nodeType) {
    os << node_type_to_string(nodeType);
    return os;
}

#endif //LANG_ASTNODE_HPP
