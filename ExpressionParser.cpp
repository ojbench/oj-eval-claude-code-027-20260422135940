#include "ExpressionParser.hpp"
#include <cctype>
#include <iostream>
#include <algorithm>

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < input.size()) {
        if (std::isspace(input[i])) {
            i++;
            continue;
        }

        if (std::isdigit(input[i])) {
            std::string val;
            while (i < input.size() && std::isdigit(input[i])) {
                val += input[i++];
            }
            tokens.push_back({TokenType::NUMBER, val});
        } else if (std::isalpha(input[i])) {
            std::string val;
            while (i < input.size() && std::isalnum(input[i])) {
                val += static_cast<char>(std::toupper(input[i++]));
            }
            tokens.push_back({TokenType::IDENTIFIER, val});
        } else if (input[i] == '+') {
            tokens.push_back({TokenType::PLUS, "+"});
            i++;
        } else if (input[i] == '-') {
            tokens.push_back({TokenType::MINUS, "-"});
            i++;
        } else if (input[i] == '*') {
            tokens.push_back({TokenType::STAR, "*"});
            i++;
        } else if (input[i] == '/') {
            tokens.push_back({TokenType::SLASH, "/"});
            i++;
        } else if (input[i] == '(') {
            tokens.push_back({TokenType::LPAREN, "("});
            i++;
        } else if (input[i] == ')') {
            tokens.push_back({TokenType::RPAREN, ")"});
            i++;
        } else if (input[i] == '<') {
            if (i + 1 < input.size() && input[i + 1] == '=') {
                tokens.push_back({TokenType::LE, "<="});
                i += 2;
            } else {
                tokens.push_back({TokenType::LT, "<"});
                i++;
            }
        } else if (input[i] == '>') {
            if (i + 1 < input.size() && input[i + 1] == '=') {
                tokens.push_back({TokenType::GE, ">="});
                i += 2;
            } else {
                tokens.push_back({TokenType::GT, ">"});
                i++;
            }
        } else if (input[i] == '=') {
            if (i + 1 < input.size() && input[i + 1] == '=') {
                tokens.push_back({TokenType::EQ, "=="});
                i += 2;
            } else {
                tokens.push_back({TokenType::EQ, "="}); // Basic uses = for equality in IF
                i++;
            }
        } else if (input[i] == '!') {
            if (i + 1 < input.size() && input[i + 1] == '=') {
                tokens.push_back({TokenType::NE, "!="});
                i += 2;
            } else {
                i++; // Ignore stray !
            }
        } else if (input[i] == '<' && i + 1 < input.size() && input[i + 1] == '>') {
             tokens.push_back({TokenType::NE, "<>"});
             i += 2;
        } else {
            i++; // Ignore other chars
        }
    }
    tokens.push_back({TokenType::END_OF_FILE, ""});
    return tokens;
}

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens, ScopeManager& scopeManager)
    : tokens(tokens), pos(0), scopeManager(scopeManager) {}

long long ExpressionParser::parse() {
    return expression();
}

long long ExpressionParser::expression() {
    return equality();
}

long long ExpressionParser::equality() {
    long long val = comparison();
    while (match(TokenType::EQ) || match(TokenType::NE)) {
        TokenType type = tokens[pos - 1].type;
        long long right = comparison();
        if (type == TokenType::EQ) val = (val == right);
        else val = (val != right);
    }
    return val;
}

long long ExpressionParser::comparison() {
    long long val = term();
    while (match(TokenType::LT) || match(TokenType::GT) || match(TokenType::LE) || match(TokenType::GE)) {
        TokenType type = tokens[pos - 1].type;
        long long right = term();
        if (type == TokenType::LT) val = (val < right);
        else if (type == TokenType::GT) val = (val > right);
        else if (type == TokenType::LE) val = (val <= right);
        else if (type == TokenType::GE) val = (val >= right);
    }
    return val;
}

long long ExpressionParser::term() {
    long long val = factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        TokenType type = tokens[pos - 1].type;
        long long right = factor();
        if (type == TokenType::PLUS) val += right;
        else val -= right;
    }
    return val;
}

long long ExpressionParser::factor() {
    long long val = unary();
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        TokenType type = tokens[pos - 1].type;
        long long right = unary();
        if (type == TokenType::STAR) val *= right;
        else {
            if (right == 0) throw std::runtime_error("DIVIDE BY ZERO");
            val /= right;
        }
    }
    return val;
}

long long ExpressionParser::unary() {
    if (match(TokenType::MINUS)) {
        return -unary();
    }
    if (match(TokenType::PLUS)) {
        return unary();
    }
    return primary();
}

long long ExpressionParser::primary() {
    if (match(TokenType::NUMBER)) {
        try {
            return std::stoll(tokens[pos - 1].value);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("INT LITERAL OVERFLOW");
        }
    }
    if (match(TokenType::IDENTIFIER)) {
        return scopeManager.getVariable(tokens[pos - 1].value);
    }
    if (match(TokenType::LPAREN)) {
        long long val = expression();
        if (!match(TokenType::RPAREN)) {
             throw std::runtime_error("MISMATCHED PARENTHESIS");
        }
        return val;
    }
    throw std::runtime_error("SYNTAX ERROR");
}

const Token& ExpressionParser::peek() const {
    return tokens[pos];
}

const Token& ExpressionParser::advance() {
    if (pos < tokens.size()) pos++;
    return tokens[pos - 1];
}

bool ExpressionParser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}
