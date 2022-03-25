//
// Created by glover on 25/03/2022.
//

#include "type.hpp"

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
Type::Type(int x) {
    setupType(this, TypeT::i32);
    i32->val = x;
}
Type::Type(AST::FunctionDefinition* fn) {
    setupType(this, TypeT::fn);
    this->fn->val = fn;
}

// Binary operators

Type Type::add(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i32: return Type(*a.i32 + *b.i32);
            case TypeT::string: return Type(a.string->val + b.string->val);
        }
    }
}
Type Type::minus(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i32: return Type(*a.i32 - *b.i32);
        }
    }
}
Type Type::divide(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i32: return Type(*a.i32 / *b.i32);
        }
    }
}
Type Type::multiply(Type a, Type b) {
    if (a.type == b.type) {
        switch (a.type) {
            case TypeT::i32: return Type(*a.i32 * *b.i32);
        }
    }
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
    return {{}, this};
}