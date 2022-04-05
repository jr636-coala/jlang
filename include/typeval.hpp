//
// Created by glover on 25/03/2022.
//

#ifndef LANG_TYPE_HPP
#define LANG_TYPE_HPP

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <optional>
#include "typet.hpp"
#include "astnode.hpp"

template <TypeT T>
struct TypeI {};


struct TypeVal {
    TypeVal(TypeT type);
    TypeVal();

    TypeVal(const std::string &str);

    TypeVal(std::int64_t x);

    TypeVal(AST::Node node);

    TypeT type;
    bool constant {};

    static TypeVal plus(TypeVal a, TypeVal b);
    static TypeVal minus(TypeVal a, TypeVal b);
    static TypeVal slash(TypeVal a, TypeVal b);
    static TypeVal star(TypeVal a, TypeVal b);

    static TypeVal plusEqual(TypeVal& a, TypeVal b);
    static TypeVal minusEqual(TypeVal& a, TypeVal b);

    union {
        TypeI<TypeT::i8>* i8;
        TypeI<TypeT::i16>* i16;
        TypeI<TypeT::i32>* i32;
        TypeI<TypeT::i64>* i64;

        TypeI<TypeT::u8>* u8;
        TypeI<TypeT::u16>* u16;
        TypeI<TypeT::u32>* u32;
        TypeI<TypeT::u64>* u64;

        TypeI<TypeT::f32>* f32;
        TypeI<TypeT::f64>* f64;

        TypeI<TypeT::c>* c;
        TypeI<TypeT::string>* string;

        TypeI<TypeT::arr>* arr;

        TypeI<TypeT::pointer>* pointer;
        TypeI<TypeT::reference>* reference;

        TypeI<TypeT::ns>* ns;
        TypeI<TypeT::pod>* pod;
        TypeI<TypeT::fn>* fn;
    };
};

template <>
struct TypeI<TypeT::i8> {
    std::int8_t val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::i16> {
    std::int16_t val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::i32> {
    std::int32_t val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::i64> {
    std::int64_t val;
    operator auto&() { return val; }
};

template <>
struct TypeI<TypeT::u8> {
    std::uint8_t val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::u16> {
    std::uint16_t val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::u32> {
    std::uint32_t val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::u64> {
    std::uint64_t val;
    operator auto&() { return val; }
};

template <>
struct TypeI<TypeT::f32> {
    float val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::f64> {
    double val;
    operator auto&() { return val; }
};

static_assert(sizeof(float) == 4); // 32 bit
static_assert(sizeof(double) == 8); // 64 bit

template <>
struct TypeI<TypeT::c> {
    char val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::string> {
    std::string val;
    operator auto&() { return val; }
};

static_assert(sizeof(char) == 1); // 8 bit

template <>
struct TypeI<TypeT::arr> {
    std::vector<TypeVal> val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::pointer> {
    TypeVal* val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::reference> {
    TypeVal* val;
    operator auto&() { return val; }
};

template <>
struct TypeI<TypeT::ns> {
    std::unordered_map<std::string, TypeVal> val;
    TypeI<TypeT::ns>* parentScope {};
    operator auto&() { return val; }

    std::pair<std::optional<TypeVal>, TypeI<TypeT::ns>*> search(std::string str);
};

template <>
struct TypeI<TypeT::pod> {
    struct { std::string name; TypeI<TypeT::ns> content; } val;
    operator auto&() { return val; }
};

template <>
struct TypeI<TypeT::fn> {
    TypeVal type;
    AST::Node val;
    operator auto&() { return val; }
};


#undef TYPE_FUNC
// #undef TYPES // FIXME
#endif //LANG_TYPE_HPP
