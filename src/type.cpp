//
// Created by glover on 25/03/2022.
//

#include "type.hpp"
#include "log.hpp"
#include "tokeniser.hpp"

void setupType(Type* type, TypeT tt) {
    type->type = tt;
    switch (tt) {
    #define TYPE_FUNC(T, S) case TypeT::T: type->T = new TypeI<TypeT::T>(); break;
        TYPES(TYPE_FUNC)
    }
}

Type::Type(TypeT type) {
    setupType(this, type);
}

Type::Type() : type(TypeT::unknown) {}

Type::Type(const std::string& str) {
    setupType(this, TypeT::string);
    string->val = str;
}
Type::Type(std::int64_t x) {
    setupType(this, TypeT::i64);
    i64->val = x;
}
//Type::Type(AST::FunctionDefinition* fn) {
//    setupType(this, TypeT::fn);
//    this->fn->val = fn;
//}

// Binary operators

void logOperatorError(Token token, Type a, Type b) {
    Log::error("No " + token_to_string(token) + " operator defined for " + typeT_to_string(a.type) + " with " + typeT_to_string(b.type), {});
}

Type Type::plus(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return Type(*a.i64 + *b.i64);
            case TypeT::string: return Type(a.string->val + b.string->val);
        }
    }
    else {
        if (a.type == TypeT::string && b.type == TypeT::i64) return Type(a.string->val + std::to_string(*b.i64));
        if (a.type == TypeT::i64 && b.type == TypeT::string) return Type(std::to_string(*a.i64) + b.string->val);
    }
    logOperatorError(Token::plus, a, b);
}
Type Type::minus(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return Type(*a.i64 - *b.i64);
        }
    }
    logOperatorError(Token::minus, a, b);
}
Type Type::slash(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return Type(*a.i64 / *b.i64);
        }
    }
    logOperatorError(Token::slash, a, b);
}
Type Type::star(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return Type(*a.i64 * *b.i64);
        }
    }
    logOperatorError(Token::star, a, b);
}

Type Type::plusEqual(Type& a, Type b) {
    if (a.type == b.type) {
        return a = Type::plus(a, b);
    }
    logOperatorError(Token::plusequal, a, b);
}
Type Type::minusEqual(Type& a, Type b) {
    if (a.type == b.type) {
        return a = Type::minus(a, b);
    }
    logOperatorError(Token::minusequal, a, b);
}

std::pair<std::optional<Type>, TypeI<TypeT::ns>*> TypeI<TypeT::ns>::search(std::string str) {
    auto f = str.find("::");
    auto nest = f != std::string::npos;
    auto first = str.substr(0, nest ? f : str.length());
    std::optional<Type> ret = {};
    if (val.contains(first)) {
        auto var = val[first];
        if (nest) {
            switch (var.type) {
                case TypeT::ns: return var.ns->search(str.substr(f + 2));
                case TypeT::pod: return var.pod->val.content.search(str.substr(f + 2));
            }
            return {{}, this};
        }
        return {var, this};
    }
    if (parentScope) return parentScope->search(str);
    return {{}, this};
}