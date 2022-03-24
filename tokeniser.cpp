//
// Created by glover on 08/03/2022.
//

#include "tokeniser.hpp"
#include "log.hpp"

Tokeniser::Tokeniser(const std::string& src, const std::string& filename) : src(src), filename(filename) {}
std::vector<TokenInfo> Tokeniser::getTokens() {
    std::vector<TokenInfo> list {};
    std::size_t index = 0;
    auto match_token = [&index, this]() {
        for (std::size_t i = 0, j = 0; i < TOKEN_MAP.size(); ++i) {
            auto[match, token, loc] = TOKEN_MAP[i];
            if (!match.length()) continue;
            for (; j < match.length(); ++j) {
                if (src[index + j] != match[j]) break;
            }
            if (j == match.length()) {
                index += match.length();
                return token;
            }
        }
        return Token::null;
    };
    auto skip_whitespace = [&index, this]() {
        while (isspace(src[index])) {
            if (src[index] == '\n') {
                line++;
                col = 0;
            }
            (++col, ++index);
        }
    };

    auto skipSingleComment = [&index, this]() {
        while (src[index] != '\n') index++;
        (col=0,++line);
    };

    auto isValidIdentifierStart = [](auto c) {
        return isalpha(c) || c == '_' || c == '$' || c == '#';
    };
    auto isValidIdentifierChar = [&isValidIdentifierStart](auto c) {
        return isValidIdentifierStart(c) || isdigit(c);
    };

    auto getString = [&index, this]() {
        std::string str;
        while (src[(++col, ++index)] != '"') str += src[index];
        // We stop on a " char so skip this one
        index++;
        return str;
    };
    auto getNumber = [&index, this]() {
        std::string str;
        str += src[index];
        while (isalnum(src[(++col,++index)]) || src[index] == '.') str += src[index];
        return str;
    };
    auto getIdentifier = [&isValidIdentifierChar, &index, this]() {
        std::string str;
        str += src[index];
        while(isValidIdentifierChar(src[(++col,++index)])) {
            str += src[index];
        }
        return str;
    };

    while (index < src.length()) {
        skip_whitespace();
        if (src[index] == '/' && src[index + 1] == '/') {
            skipSingleComment();
            continue;
        }
        auto token = match_token();
        // Set token location
        Loc loc = {
                .filename = filename,
                .line = line,
                .col = col
        };
        std::string str = "";
        if (token == Token::null) {
            if (src[index] == '"') {
                token = Token::string;
                str = getString();
            }
            else if (isdigit(src[index])) {
                token = Token::number;
                str = getNumber();
            }
            else if (isValidIdentifierStart(src[index])) {
                token = Token::identifier;
                str = getIdentifier();
            }
            else {
                Log::error(std::string("Invalid token \"") + src[index] + "\"", { .filename = filename, .line = line, .col = col });
                (++col,++index);
            }
        }

        list.push_back({ .match = str, .token = token, .loc = loc });
    }
    return list;
}
