#pragma once

#include <cassert>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <variant>

#include "parser.hpp"

class Generator {
private:
    struct Var {
        size_t stack_loc;
    };

private:
    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> vars;

public:
    Generator(const NodeProg &prog) : m_prog(prog) {}

    void generate_expr(const NodeExpr &expr) {
        struct ExprVisitor {
            Generator *gen;

            void operator()(const NodeExprIntLit &expr) {
                gen->m_output << "    mov X0, #" << expr.int_lit.value.value() << "\n";
                gen->push("X0");
            }

            void operator()(const NodeExprIdent &expr) {
                auto ident = expr.identifier.value.value();
                if (gen->vars.find(ident) == gen->vars.end()) {
                    std::cerr << "Identifier `" << ident << "` not found\n";
                    exit(EXIT_FAILURE);
                }

                size_t offset = gen->m_stack_size - gen->vars[ident].stack_loc - 1;
                std::stringstream reg;
                reg << "[sp, #0x" << std::hex << offset*16 << "]";
                gen->m_output << "    ldr X0, " << reg.str() << "\n";
                gen->push("X0");
            }
        };

        ExprVisitor visitor { .gen = this };
        std::visit(visitor, expr.var);
    }

    void generate_stmt(const NodeStmt &stmt) {
        struct StmtVisitor {
            Generator *gen;

            void operator()(const NodeStmtExit &stmt) {
                gen->generate_expr(stmt.expr);
                gen->pop("X0");
                gen->m_output << "    mov X16, #1\n"
                              << "    svc #0x80\n";
            }

            void operator()(const NodeStmtLet &stmt) {
                auto ident = stmt.identifier.value.value();
                if (gen->vars.find(ident) != gen->vars.end()) {
                    std::cerr << "`" << ident << "`"
                              << " identifier already used\n";
                    exit(EXIT_FAILURE);
                }

                gen->generate_expr(stmt.expr);
                gen->vars.insert({
                    ident,
                    Var { .stack_loc = gen->m_stack_size },
                });
                ++gen->m_stack_size;
            }

            void operator()(const NodeStmtPrint &stmt) {
                // static_assert(false, "not implemented!");

                gen->generate_expr(stmt.expr);
                gen->pop("X0");
                gen->m_output << "    mov X0, #1\n"
                              << "    adr X1, #10\n"
                              << "    mov X16, #" << 0 << "\n"
                              << "    svc #0x80\n";
            }
        };

        StmtVisitor visitor { .gen = this };
        std::visit(visitor, stmt.var);
    }

    void generate_prog() {
        m_output << ".global _start\n"
                 << ".align 2\n\n"
                 << "_start:\n";

        for (const NodeStmt &stmt : m_prog.stmts) {
            generate_stmt(stmt);
        }

        // exit the program with 0 at the end
        m_output << "\n"
                 << "    mov X0, #0\n"
                 << "    mov X16, #1\n"
                 << "    svc #0x80\n";
    }

    std::string get_string() const { return m_output.str(); }

private:
    void push(const std::string &reg) {
        m_output << "    str " << reg << ", [sp, #-0x10]!\n";
        ++m_stack_size;
    }

    void pop(const std::string &reg) {
        m_output << "    ldr " << reg << ", [sp]\n";
        --m_stack_size;
    }
};
