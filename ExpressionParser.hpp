#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <string>
#include <vector>
#include "ScopeManager.hpp"

enum class TokenType {
    NUMBER,
    IDENTIFIER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    LT,
    GT,
    LE,
    GE,
    EQ,
    NE,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
};

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens, ScopeManager& scopeManager);
    long long parse();

private:
    std::vector<Token> tokens;
    size_t pos;
    ScopeManager& scopeManager;

    long long expression();
    long long equality();
    long long comparison();
    long long term();
    long long factor();
    long long unary();
    long long primary();

    const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
};

std::vector<Token> tokenize(const std::string& input);

#endif
