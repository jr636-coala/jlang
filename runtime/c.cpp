#include <cstdio>
#include <cstring>
#include "typeval.hpp"

extern "C" {
    TypeVal jl_puts(const TypeVal& x) { return TypeVal(std::puts(x.string->val.c_str())); }
    TypeVal jl_strlen(const TypeVal& x) { return TypeVal(std::strlen(x.string->val.c_str())); }
    TypeVal jl_fopenw(const TypeVal& x) { return TypeVal((int64_t)std::fopen(x.string->val.c_str(), "w")); }
    TypeVal jl_fclose(const TypeVal& x) { return TypeVal(std::fclose((FILE*)x.i64->val)); }
    TypeVal jl_fwrite(const TypeVal& content, const TypeVal& file) { return TypeVal(std::fwrite(content.string->val.c_str(), content.string->val.length(), 1, (FILE*)(file.i64->val))); }
    TypeVal jl_system(const TypeVal& x) { return TypeVal(std::system(x.string->val.c_str())); }
}