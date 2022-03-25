//
// Created by glover on 08/03/2022.
//

#ifndef LANG_TOKENISER_HPP
#define LANG_TOKENISER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <experimental/array>
#include "loc.hpp"

#define TOKENS(_) \
  _(null, "") \
  _(lparen, "(") \
  _(rparen, ")") \
  _(lcurly, "{") \
  _(rcurly, "}") \
  _(lsquare, "[") \
  _(rsquare, "]") \
  _(semicolon, ";") \
  _(comma, ",")   \
  _(plusequal, "+=") \
  _(plus, "+")    \
  _(minusequal, "-=") \
  _(minus, "-")   \
  _(perc, "%")    \
  _(star, "*")    \
  _(slash, "/")   \
  _(bslah, "\\")  \
  _(dcolon, "::") \
  _(colon, ":")\
  _(string, "")   \
  _(equal, "==") \
  _(assign, "=")  \
  _(i8, "i8") \
  _(i16, "i16") \
  _(i32, "i32") \
  _(i64, "i64") \
  _(u8, "u8") \
  _(u16, "u16") \
  _(u32, "u32") \
  _(u64, "u64")   \
  _(f32, "f32")   \
  _(f64, "f64")   \
  _(c, "char")    \
  _(str, "string") \
  _(identifier, "") \
  _(fn, "fn") \
  _(tif, "if")    \
  _(telse, "else") \
  _(twhile, "while")\
  _(let, "let")  \
  _(tconst, "const")\
  _(number, "") \
  _(ret, "return") \
  _(pod, "pod")

enum class Token {
#define TOKEN_FUNC(T, S) T,
    TOKENS(TOKEN_FUNC)
};

constexpr auto TypeTokens = {
        Token::i8,
        Token::i16,
        Token::i32,
        Token::i64,

        Token::u8,
        Token::u16,
        Token::u32,
        Token::u64,

        Token::f32,
        Token::f64,

        Token::c,
        Token::str
};

inline std::string token_to_string(Token token) {
    switch (token) {
#define TOKEN_FUNC(T, S) case(Token::T): return #T;
        TOKENS(TOKEN_FUNC)
        default: return "# BROKEN : INVALID TOKEN #";
    }
}

inline std::ostream& operator<<(std::ostream& os, Token token) {
    os << token_to_string(token);
    return os;
}

struct TokenInfo {
    std::string match;
    Token token;
    Loc loc;

    operator Token() const { return token; }
};

const auto TOKEN_MAP = std::to_array<TokenInfo>({
#define TOKEN_FUNC(T, S) {S, Token::T},
    TOKENS(TOKEN_FUNC)
});

class Tokeniser {
public:
    Tokeniser(const std::string& src, const std::string& filename = "");
    std::vector<TokenInfo> getTokens();

private:
    std::string src {};
    std::size_t line {1}; // New lines in strings will mess this up
    std::size_t col {1};
    std::string filename {};
};


#endif //LANG_TOKENISER_HPP
