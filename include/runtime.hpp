//
// Created by glover on 25/03/2022.
//

#ifndef LANG_C_HPP
#define LANG_C_HPP

#include "typeval.hpp"

namespace JL_C {
    extern "C" {
        TypeVal jl_puts(const TypeVal&);
        TypeVal jl_strlen(const TypeVal&);

        TypeVal jl_fopenw(const TypeVal&);
        TypeVal jl_fclose(const TypeVal&);
        TypeVal jl_fwrite(const TypeVal&, const TypeVal&);

        TypeVal jl_system(const TypeVal&);
    }
}
#endif //LANG_C_HPP
