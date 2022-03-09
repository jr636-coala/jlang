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
  _(plus, "+")    \
  _(minus, "-")   \
  _(perc, "%")    \
  _(star, "*")    \
  _(slash, "/")   \
  _(bslah, "\\")  \
  _(dcolon, "::") \
  _(colon, ":")\
  _(string, "")   \
  _(equal, "==")                \
  _(assign, "=")                \
  _(identifier, "") \
  _(fn, "fn") \
  _(tif, "if") \
  _(twhile, "while")\
  _(let, "let")  \
  _(tconst, "const")\
  _(number, "")

enum class Token {
#define TOKEN_FUNC(T, S) T,
    TOKENS(TOKEN_FUNC)
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
