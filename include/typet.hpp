//
// Created by glover on 29/03/2022.
//

#ifndef LANG_TYPET_HPP
#define LANG_TYPET_HPP

#include <string>

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
    TYPES(TYPE_FUNC)E
};

inline std::string typeT_to_string(TypeT type) {
    switch (type) {
#define TYPE_FUNC(T, S) case(TypeT::T): return S;
        TYPES(TYPE_FUNC)
        default: return "unknown";
    }
}

#endif //LANG_TYPET_HPP
