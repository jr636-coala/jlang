//
// Created by glover on 25/03/2022.
//

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>

extern "C" {
    std::uint64_t jl_puts(const char* str) {
        return std::puts(str);
    };
    std::uint64_t jl_strlen(const char* str) {
        return std::strlen(str);
    }
    std::uint64_t jl_fopenw(const char* str) {
        auto file = std::fopen(str, "wb");
        return reinterpret_cast<int64_t>(file);
    }
    std::uint64_t jl_fclose(std::int64_t file) {
        return std::fclose(reinterpret_cast<FILE*>(file));
    }
    std::uint64_t jl_fwrite(const char* str, std::int64_t len, std::int64_t file) {
        return std::fwrite(str, sizeof(char)*len, 1, (FILE*)file);
    }
    std::uint64_t jl_system(const char* str) {
        return std::system(str);
    }
}