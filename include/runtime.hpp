//
// Created by glover on 25/03/2022.
//

#ifndef LANG_C_HPP
#define LANG_C_HPP

#include "type.hpp"

namespace JL_C {
    extern "C" {
        uint64_t jl_puts(const Type&);
        uint64_t jl_strlen(const Type&);

        uint64_t jl_fopenw(const Type&);
        uint64_t jl_fclose(const Type&);
        uint64_t jl_fwrite(const Type&, const Type&);

        uint64_t jl_system(const Type&);
    }
}
#endif //LANG_C_HPP
