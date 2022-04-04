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
        _(null) \
        _(plus) \
        _(minus) \
        _(uplus) \
        _(uminus) \
        _(assign) \
        _(plusassign) \
        _(minusassign) \
        _(equal) \
        _(slash) \
        _(star) \
        _(perc) \
        _(deref) \
        _(addr) \
        _(andd) \
        _(orr) \
        _(pod_access) \
        _(call) \
        _(index) \
        _(ns) \
        _(number) \
        _(string) \
        _(identifier) \
        \
        _(func_def) \
        _(pod_def) \
        _(iff) \
        _(whilee) \
        _(returnn) \
        _(let) \
        _(constt) \
        _(list)

#define AST_IDENTIFIER_TYPES(_) \
        _(unknown) \
        _(type) \
        _(function) \
        _(variable) \
        _(parameter)

namespace AST {

    struct Identifier {
        #define _(x) x,
        enum class Type { AST_IDENTIFIER_TYPES(_) };
        #undef _

        Type type;
        std::string name;

        TypeName valType;
    };

    struct Node {
        #define _(x) x,
        enum class Type { AST_NODE_TYPES(_) };
        #undef _

        Node() : type(Type::null) {}
        Node(Type type) : type(type) {}

        static std::string type_to_string(Type type) {
            #define _(x) case Type::x: return #x;
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
            }
            Node node(mappedOp);
            node.params.push_back(a);
            node.params.push_back(b);
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
        TypeName valType;
    };
}

inline std::ostream& operator<<(std::ostream& os, AST::Node::Type nodeType) {
    os << AST::Node::type_to_string(nodeType);
    return os;
}

#endif //LANG_ASTNODE_HPP
