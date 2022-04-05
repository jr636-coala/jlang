#include <cstdio>
#include <cstring>
#include "typeval.hpp"

extern "C" {
    uint64_t jl_puts(const TypeVal& x) { return std::puts(x.string->val.c_str()); }
    uint64_t jl_strlen(const TypeVal& x) { return std::strlen(x.string->val.c_str()); }
    uint64_t jl_fopenw(const TypeVal& x) { return (uint64_t)std::fopen(x.string->val.c_str(), "w"); }
    uint64_t jl_fclose(const TypeVal& x) { return std::fclose((FILE*)x.i64); }
    uint64_t jl_fwrite(const TypeVal& content, const TypeVal& file) { return std::fwrite(content.string->val.c_str(), content.string->val.length(), 1, (FILE*)file.i64); }
    uint64_t jl_system(const TypeVal& x) { return std::system(x.string->val.c_str()); }
}