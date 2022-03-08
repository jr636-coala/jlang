//
// Created by glover on 08/03/2022.
//

#ifndef LANG_TOKENISER_HPP
#define LANG_TOKENISER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <experimental/array>

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
  _(string, "") \
  _(identifier, "") \
  _(fn, "fn") \
  _(tif, "if") \
  _(twhile, "while") \
  _(number, "") \

enum class Token {
#define TOKEN_FUNC(T, S) T,
    TOKENS(TOKEN_FUNC)
};

inline std::ostream& operator<<(std::ostream& os, Token token) {
    switch (token) {
#define TOKEN_FUNC(T, S) case(Token::T): os << #T; break;
        TOKENS(TOKEN_FUNC)
        default: os << "# BROKEN : INVALID TOKEN #"; break;
    }
    return os;
}

struct TokenInfo {
    std::string match;
    Token token;
};

const auto TOKEN_MAP = std::to_array<TokenInfo>({
#define TOKEN_FUNC(T, S) {S, Token::T},
    TOKENS(TOKEN_FUNC)
});

class Tokeniser {
public:
    Tokeniser(const std::string& src);
    std::vector<TokenInfo> getTokens();

private:
    std::string src {};
};


#endif //LANG_TOKENISER_HPP
