//
// Created by glover on 08/03/2022.
//

#ifndef LANG_LOG_HPP
#define LANG_LOG_HPP

#include <string>
#include <iostream>
#include "loc.hpp"

using namespace std::literals::string_literals;

#define FATAL_ERROR(msg) Log::fatalError(msg, __LINE__, __FILE__, __func__)

#define COLOR(c) "\033[" str(c) "m"
#define RESET "\033[0m"

#define str(x) str_(x)
#define str_(x) #x

#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37

namespace Log {
    enum class LEVEL { VERBOSE, LOG, ERROR };

    //enum class Color { black, red, green, yellow, blue, magenta, cyan, white };
    //enum class Decoration { reset = 0, bold = 1, underline = 4, inverse = 7, bold_off = 21, underline_off = 24, inverse_off = 27 };


    void fatalError(auto msg, auto line, auto file, auto func) {
        std::cout << COLOR(RED) << "FATAL ERROR\n\t" <<
            msg << '\n'
            << file << "::" << func << "::ln" << line << '\n' << RESET;
        exit(-1);
    }

    namespace {
        std::string _msg(const std::string& level, const auto& type, const auto& msg, const Loc& loc) {
            return
                level + "(" + type + ")" + "\n\t" +
                msg + '\n' +
                static_cast<std::string>(loc) + '\n' +
                RESET;
        }
    }

    void error(auto msg, const Loc& loc, const std::string& type = "") { std::cout << COLOR(MAGENTA) << _msg("ERROR", type, msg, loc); }
    void warning(auto msg, const Loc& loc, const std::string& type = "") { std::cout << COLOR(YELLOW) << _msg("WARNING", type, msg, loc); }
    void log(auto msg, const Loc& loc) { std::cout << COLOR(WHITE) << _msg("LOG", "log", msg, loc); }
};

#undef COLOR
#undef RESET

#endif //LANG_LOG_HPP
