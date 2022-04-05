//
// Created by glover on 25/03/2022.
//

#include "typeval.hpp"
#include "log.hpp"
#include "tokeniser.hpp"

void setupType(TypeVal* type, TypeT tt) {
    type->type = tt;
    switch (tt) {
    #define TYPE_FUNC(T, S) case TypeT::T: type->T = new TypeI<TypeT::T>(); break;
        TYPES(TYPE_FUNC)
    }
}

TypeVal::TypeVal(TypeT type) {
    setupType(this, type);
}

TypeVal::TypeVal() : type(TypeT::unknown) {}

TypeVal::TypeVal(const std::string& str) {
    setupType(this, TypeT::string);
    string->val = str;
}
TypeVal::TypeVal(std::int64_t x) {
    setupType(this, TypeT::i64);
    i64->val = x;
}
TypeVal::TypeVal(AST::Node node) {
    setupType(this, TypeT::fn);
    this->fn->val = node;
}

// Binary operators

void logOperatorError(Token token, TypeVal a, TypeVal b) {
    Log::error("No " + token_to_string(token) + " operator defined for " + typeT_to_string(a.type) + " with " + typeT_to_string(b.type), {});
}

TypeVal TypeVal::plus(TypeVal a, TypeVal b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return TypeVal(*a.i64 + *b.i64);
            case TypeT::string: return TypeVal(a.string->val + b.string->val);
        }
    }
    else {
        if (a.type == TypeT::string && b.type == TypeT::i64) return TypeVal(a.string->val + std::to_string(*b.i64));
        if (a.type == TypeT::i64 && b.type == TypeT::string) return TypeVal(std::to_string(*a.i64) + b.string->val);
    }
    logOperatorError(Token::plus, a, b);
}
TypeVal TypeVal::minus(TypeVal a, TypeVal b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return TypeVal(*a.i64 - *b.i64);
        }
    }
    logOperatorError(Token::minus, a, b);
}
TypeVal TypeVal::slash(TypeVal a, TypeVal b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return TypeVal(*a.i64 / *b.i64);
        }
    }
    logOperatorError(Token::slash, a, b);
}
TypeVal TypeVal::star(TypeVal a, TypeVal b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i64: return TypeVal(*a.i64 * *b.i64);
        }
    }
    logOperatorError(Token::star, a, b);
}

TypeVal TypeVal::plusEqual(TypeVal& a, TypeVal b) {
    if (a.type == b.type) {
        return a = TypeVal::plus(a, b);
    }
    logOperatorError(Token::plusequal, a, b);
}
TypeVal TypeVal::minusEqual(TypeVal& a, TypeVal b) {
    if (a.type == b.type) {
        return a = TypeVal::minus(a, b);
    }
    logOperatorError(Token::minusequal, a, b);
}

std::pair<std::optional<TypeVal>, TypeI<TypeT::ns>*> TypeI<TypeT::ns>::search(std::string str) {
    auto f = str.find("::");
    auto nest = f != std::string::npos;
    auto first = str.substr(0, nest ? f : str.length());
    std::optional<TypeVal> ret = {};
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