//
// Created by glover on 25/03/2022.
//

#ifndef LANG_TYPE_HPP
#define LANG_TYPE_HPP

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>
#include "astnode.hpp"
#include <memory>
#include <optional>

#define TYPES(_) \
  _(i8, "i8")\
  _(i16, "i16")\
  _(i32, "i32")\
  _(i64, "i64")  \
  _(u8, "u8")\
  _(u16, "u16")\
  _(u32, "u32")\
  _(u64, "u64")  \
  _(f32, "f32")  \
  _(f64, "f64")  \
  _(c, "char")   \
  _(string, "string")   \
  _(arr, "[]")   \
  _(pointer, "*")\
  _(reference, "&") \
  _(ns, "ns")    \
  _(pod, "pod") \
  _(fn, "fn")

enum class TypeT {
    unknown,
#define TYPE_FUNC(T, S) T,
    TYPES(TYPE_FUNC)
};

inline std::string typeT_to_string(TypeT type) {
    switch (type) {
#define TYPE_FUNC(T, S) case(TypeT::T): return #T;
        TYPES(TYPE_FUNC)
        default: return "# BROKEN : INVALID TYPE_T #";
    }
}

template <TypeT T>
struct TypeI {};


struct Type {
    Type(TypeT type);
    Type();

    Type(const std::string &str);

    Type(std::int64_t x);

    Type(AST::FunctionDefinition *fn);

    TypeT type;
    bool constant {};

    static Type plus(Type a, Type b);
    static Type minus(Type a, Type b);
    static Type slash(Type a, Type b);
    static Type star(Type a, Type b);

    static Type plusEqual(Type& a, Type b);
    static Type minusEqual(Type& a, Type b);

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
    std::vector<Type> val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::pointer> {
    Type* val;
    operator auto&() { return val; }
};
template <>
struct TypeI<TypeT::reference> {
    Type* val;
    operator auto&() { return val; }
};

template <>
struct TypeI<TypeT::ns> {
    std::unordered_map<std::string, Type> val;
    TypeI<TypeT::ns>* parentScope {};
    operator auto&() { return val; }

    std::pair<std::optional<Type>, TypeI<TypeT::ns>*> search(std::string str);
};

template <>
struct TypeI<TypeT::pod> {
    struct { std::string name; TypeI<TypeT::ns> content; } val;
    operator auto&() { return val; }
};

template <>
struct TypeI<TypeT::fn> {
    AST::FunctionDefinition* val;
    operator auto&() { return val; }
};


#undef TYPE_FUNC
// #undef TYPES // FIXME
#endif //LANG_TYPE_HPP
