#pragma once

#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum class TokenType {
    Exit,
    IntLiteral,
    SemiColon,
    LParen,
    RParen,
    Ident,
    Let,
    Eq,
};

struct Token {
    TokenType type;
    std::optional<std::string> value = {};
};

inline std::string token_to_string(const Token &token) {
    switch (token.type) {
        case TokenType::Exit:
            return "Exit";
        case TokenType::IntLiteral:
            return "IntLiteral";
        case TokenType::SemiColon:
            return "SemiColon";
        case TokenType::LParen:
            return "LParen";
        case TokenType::RParen:
            return "RParen";
        case TokenType::Ident:
            return "Ident";
        case TokenType::Let:
            return "Let";
        case TokenType::Eq:
            return "Eq";
        default:
            return "Invalid";
    }
}

class Tokenizer {
private:
    const std::string m_src;
    size_t m_index = 0;

public:
    Tokenizer(const std::string& src)
        : m_src(src) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buf;

        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                while (std::isalpha(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "exit") {
                    tokens.push_back({ .type = TokenType::Exit });
                } else if (buf == "let") {
                    tokens.push_back({ .type = TokenType::Let });
                } else {
                    tokens.push_back({ .type = TokenType::Ident, .value = buf });
                }
                buf.clear();
            } else if (std::isdigit(peek().value())) {
                while (std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::IntLiteral, .value = buf });
                buf.clear();
            } else if (peek().value() == '(') {
                tokens.push_back({ .type = TokenType::LParen });
                consume();
            } else if (peek().value() == ')') {
                tokens.push_back({ .type = TokenType::RParen });
                consume();
            } else if (peek().value() == ';') {
                tokens.push_back({ .type = TokenType::SemiColon });
                consume();
            } else if (peek().value() == '=') {
                tokens.push_back({ .type = TokenType::Eq });
                consume();
            } else {
                consume();
            }
        }

        return tokens;
    }

    char consume() {
        return m_src[m_index++];
    }

    std::optional<char> peek() const {
        if (m_index >= m_src.size()) {
            return {};
        }
        return m_src[m_index];
    }
};
