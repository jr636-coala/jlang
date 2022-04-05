//
// Created by glover on 25/03/2022.
//

#ifndef LANG_C_HPP
#define LANG_C_HPP

#include "typeval.hpp"

namespace JL_C {
    extern "C" {
        uint64_t jl_puts(const TypeVal&);
        uint64_t jl_strlen(const TypeVal&);

        uint64_t jl_fopenw(const TypeVal&);
        uint64_t jl_fclose(const TypeVal&);
        uint64_t jl_fwrite(const TypeVal&, const TypeVal&);

        uint64_t jl_system(const TypeVal&);
    }
}
#endif //LANG_C_HPP
