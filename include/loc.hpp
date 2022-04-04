//
// Created by glover on 08/03/2022.
//

#ifndef LANG_LOC_HPP
#define LANG_LOC_HPP

#include <string>

struct Loc {
    std::string filename;
    std::size_t line;
    std::size_t col;

    operator std::string() const {
        return filename + "::ln" + std::to_string(line) + ',' + std::to_string(col);
    }
};

#endif //LANG_LOC_HPP
