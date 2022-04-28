//
// Created by glover on 08/03/2022.
//

#ifndef LANG_ASTNODE_HPP
#define LANG_ASTNODE_HPP

#include <string>
#include <ostream>
#include <vector>
#include <optional>
#include "loc.hpp"
#include "tokeniser.hpp"
#include "typet.hpp"

#define AST_NODE_TYPES(_) \
        _(null, "null") \
        _(plus, "+") \
        _(minus, "-") \
        _(uplus, "+") \
        _(uminus, "-") \
        _(assign, "=") \
        _(plusassign, "+=") \
        _(minusassign, "-=") \
        _(equal, "==") \
        _(slash, "/") \
        _(star, "*") \
        _(perc, "%") \
        _(deref, "*") \
        _(addr, "&") \
        _(andd, "&&") \
        _(orr, "||") \
        _(pod_access, ".") \
        _(call, "call") \
        _(index, "index") \
        _(ns, "ns") \
        _(number, "number") \
        _(string, "string") \
        _(identifier, "identifier") \
        \
        _(func_def, "func_def") \
        _(pod_def, "pod_def") \
        _(iff, "if") \
        _(whilee, "while") \
        _(returnn, "return") \
        _(let, "let") \
        _(constt, "const") \
        _(list, "list")

#define AST_IDENTIFIER_SCOPES(_) \
        _(unknown) \
        _(type) \
        _(function) \
        _(variable) \
        _(parameter)

namespace AST {

    struct Type {
        Type() : type(TypeT::unknown) {}
        Type(TypeT type, std::string name = "") : type(type), name(name) {}
        TypeT type;
        std::string name;
        std::vector<TypeT> modifier;
    };

    struct Identifier {
        #define _(x) x,
        enum class Scope { AST_IDENTIFIER_SCOPES(_) };
        #undef _

        Scope scope;
        std::string name;

        Type type;
    };

    struct Node {
        #define _(x,y) x,
        enum class Type { AST_NODE_TYPES(_) };
        #undef _

        Node() : type(Type::null) {}
        Node(Type type) : type(type) {}
        Node(Identifier identifier) : type(Node::Type::identifier), identifier(identifier) {}

        static std::string type_to_string(Type type) {
            #define _(x,y) case Type::x: return y;
            switch (type) {
                AST_NODE_TYPES(_)
                default: return "# UNKNOWN NODE TYPE #";
            }
            #undef _
        }

        static Node Number(std::string x) {
            Node node(Node::Type::number);
            node.value = x;
            return node;
        }
        static Node String(std::string x) {
            Node node(Node::Type::string);
            node.value = x;
            return node;
        }

        static Node BinOp(Node a, TokenInfo op, Node b) {
            auto mappedOp = Type::null;
            switch (op.token) {
                case Token::plus: mappedOp = Type::plus; break;
                case Token::minus: mappedOp = Type::minus; break;
                case Token::star: mappedOp = Type::star; break;
                case Token::slash: mappedOp = Type::slash; break;
                case Token::perc: mappedOp = Type::perc; break;

                case Token::andd: mappedOp = Type::andd; break;
                case Token::orr: mappedOp = Type::orr; break;

                case Token::assign: mappedOp = Type::assign; break;
                case Token::plusequal: mappedOp = Type::plusassign; break;
                case Token::minusequal: mappedOp = Type::minusassign; break;
            }
            Node node(mappedOp);
            node.params.push_back(a);
            node.params.push_back(b);
            node.loc = op.loc;
            return node;
        }

        static Node FuncDef() {
            Node node(Node::Type::func_def);
            node.params.push_back(Node(Node::Type::list)); // parameters
            node.params.push_back(Node(Node::Type::list)); // body
            return node;
        }

        Type type;
        std::string value;
        Identifier identifier;
        std::vector<Node> params;
        Loc loc;
        AST::Type valType;
    };
}

inline std::ostream& operator<<(std::ostream& os, AST::Type type) {
    if (type.type == TypeT::pod) os << type.name;
    else os << typeT_to_string(type.type);
    for (auto i = 0; i < type.modifier.size(); ++i) os << type.modifier[i];
    return os;
}

inline std::ostream& operator<<(std::ostream& os, AST::Identifier id) {
    os << id.name;
    if (id.type.type != TypeT::unknown) {
        os << " : " << id.type;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, AST::Node::Type nodeType) {
    os << AST::Node::type_to_string(nodeType);
    return os;
}

#endif //LANG_ASTNODE_HPP
