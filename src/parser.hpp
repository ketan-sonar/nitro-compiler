#pragma once

#include <cstdlib>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

#include "tokenizer.hpp"

struct NodeExprIntLit {
    Token int_lit;
};

struct NodeExprIdent {
    Token identifier;
};

struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdent> var;
};

struct NodeStmtExit {
    NodeExpr expr;
};

struct NodeStmtLet {
    Token identifier;
    NodeExpr expr;
};

struct NodeStmtPrint {
    NodeExpr expr;
};

struct NodeStmt {
    std::variant<NodeStmtExit, NodeStmtLet, NodeStmtPrint> var;
};

struct NodeProg {
    std::vector<NodeStmt> stmts;
};

class Parser {
private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;

public:
    Parser(const std::vector<Token> &tokens)
        : m_tokens(tokens) {}

    std::optional<NodeExpr> parse_expr() {
        if (peek().has_value()) {
            if (peek().value().type == TokenType::IntLiteral) {
                return NodeExpr { .var = NodeExprIntLit { .int_lit = consume() } };
            } else if (peek().value().type == TokenType::Ident) {
                return NodeExpr { .var = NodeExprIdent { .identifier = consume() } };
            }
        }

        return {};
    }

    std::optional<NodeStmt> parse_stmt() {
        if (peek().has_value()) {
            if (peek().value().type == TokenType::Exit &&
                peek(1).has_value() &&
                peek(1).value().type == TokenType::LParen)
            {
                consume();
                consume();

                auto expr_node = parse_expr();
                if (!expr_node.has_value()) {
                    std::cerr << "ERROR: could not parse expression inside exit\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() &&
                    peek().value().type == TokenType::RParen)
                {
                    consume();
                } else {
                    std::cerr << "ERROR: `)` expected\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() &&
                    peek().value().type == TokenType::SemiColon)
                {
                    consume();
                    return NodeStmt { .var = NodeStmtExit{ .expr = expr_node.value() } };
                } else {
                    std::cerr << "ERROR: `;` expected\n";
                    exit(EXIT_FAILURE);
                }
            } else if (peek().value().type == TokenType::Let) {
                consume();
                NodeStmtLet stmt_let;
                if (peek().has_value() &&
                    peek().value().type == TokenType::Ident)
                {
                    stmt_let.identifier = consume();
                } else {
                    std::cerr << "ERROR: `identifier` expected\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() &&
                    peek().value().type == TokenType::Eq)
                {
                    consume();
                } else {
                    std::cerr << "ERROR: `=` expected\n";
                    exit(EXIT_FAILURE);
                }

                auto expr_node = parse_expr();
                if (expr_node.has_value()) {
                    stmt_let.expr = expr_node.value();
                } else {
                    std::cerr << "ERROR: `expression` expected\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() &&
                    peek().value().type == TokenType::SemiColon)
                {
                    consume();
                    return NodeStmt { .var = stmt_let };
                } else {
                    std::cerr << "ERROR: `;` expected\n";
                    exit(EXIT_FAILURE);
                }
            } else if (peek().value().type == TokenType::Print) {
                consume();
                NodeStmtPrint stmt_print;

                if (peek().has_value() &&
                    peek().value().type == TokenType::LParen)
                {
                    consume();
                } else {
                    std::cerr << "ERROR: `(` expected\n";
                    exit(EXIT_FAILURE);
                }

                auto expr_node = parse_expr();
                if (expr_node.has_value()) {
                    stmt_print.expr = expr_node.value();
                } else {
                    std::cerr << "ERROR: `expression` expected\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() &&
                    peek().value().type == TokenType::RParen)
                {
                    consume();
                } else {
                    std::cerr << "ERROR: `)` expected\n";
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() &&
                    peek().value().type == TokenType::SemiColon)
                {
                    consume();
                    return NodeStmt { .var = stmt_print };
                } else {
                    std::cerr << "ERROR: `;` expected\n";
                    exit(EXIT_FAILURE);
                }
            }
        }

        return {};
    }

    std::optional<NodeProg> parse_prog() {
        NodeProg prog;
        while (peek().has_value()) {
            auto stmt = parse_stmt();
            if (stmt.has_value()) {
                prog.stmts.push_back(stmt.value());
            } else {
                std::cerr << "ERROR: invalid statement\n";
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

    Token consume() { return m_tokens[m_index++]; }

    std::optional<Token> peek(size_t offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        return m_tokens[m_index + offset];
    }
};
