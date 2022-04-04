#include <cstdio>
#include <cstring>
#include "type.hpp"

extern "C" {
    uint64_t jl_puts(const Type& x) { return std::puts(x.string->val.c_str()); }
    uint64_t jl_strlen(const Type& x) { return std::strlen(x.string->val.c_str()); }
    uint64_t jl_fopenw(const Type& x) { return (uint64_t)std::fopen(x.string->val.c_str(), "w"); }
    uint64_t jl_fclose(const Type& x) { return std::fclose((FILE*)x.i64); }
    uint64_t jl_fwrite(const Type& content, const Type& file) { return std::fwrite(content.string->val.c_str(), content.string->val.length(), 1, (FILE*)file.i64); }
    uint64_t jl_system(const Type& x) { return std::system(x.string->val.c_str()); }
}