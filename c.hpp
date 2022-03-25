//
// Created by glover on 25/03/2022.
//

#ifndef LANG_C_HPP
#define LANG_C_HPP

namespace JL_C {
    extern "C" {
        std::uint64_t jl_puts(const char*);
        std::uint64_t jl_strlen(const char*);

        std::uint64_t jl_fopenw(const char*);
        std::uint64_t jl_fclose(std::int64_t);
        std::uint64_t jl_fwrite(const char*, std::int64_t, std::int64_t);

        std::uint64_t jl_system(const char*);
    }
}
#endif //LANG_C_HPP
