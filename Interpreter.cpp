#include "Interpreter.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

Interpreter::Interpreter() : running(false) {}

void Interpreter::processLine(const std::string& line) {
    std::string trimmed = trim(line);
    if (trimmed.empty()) return;

    std::stringstream ss(trimmed);
    int lineNum;
    if (ss >> lineNum) {
        std::string statement;
        std::getline(ss, statement);
        statement = trim(statement);
        if (statement.empty()) {
            program.erase(lineNum);
        } else {
            program[lineNum] = statement;
        }
    } else {
        // Immediate mode
        executeStatement(trimmed);
    }
}

void Interpreter::run() {
    scopeManager.clear();
    auto it = program.begin();
    while (it != program.end()) {
        try {
            std::string statement = it->second;
            std::string cmd;
            std::stringstream ss(statement);
            ss >> cmd;
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

            if (cmd == "GOTO") {
                int target;
                if (!(ss >> target)) throw std::runtime_error("SYNTAX ERROR");
                it = program.find(target);
                if (it == program.end()) {
                    std::cout << "LINE NOT DEFINED" << std::endl;
                    return;
                }
                continue;
            } else if (cmd == "IF") {
                std::string lineStr = it->second;
                size_t thenPos = lineStr.find(" THEN ");
                if (thenPos == std::string::npos) {
                    thenPos = lineStr.find(" then ");
                }
                if (thenPos == std::string::npos) throw std::runtime_error("SYNTAX ERROR");

                std::string exprPart = lineStr.substr(3, thenPos - 3);
                std::string thenPart = lineStr.substr(thenPos + 6);

                auto tokens = tokenize(exprPart);
                ExpressionParser parser(tokens, scopeManager);
                if (parser.parse()) {
                    int target = std::stoi(trim(thenPart));
                    it = program.find(target);
                    if (it == program.end()) {
                        std::cout << "LINE NOT DEFINED" << std::endl;
                        return;
                    }
                    continue;
                } else {
                    it++;
                    continue;
                }
            } else if (cmd == "END") {
                return;
            } else {
                executeStatement(statement);
            }

            it++;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return;
        }
    }
}

// Redoing executeStatement and line processing with more care
void Interpreter::executeStatement(const std::string& statement) {
    std::stringstream ss(statement);
    std::string cmd;
    ss >> cmd;
    std::string upperCmd = cmd;
    std::transform(upperCmd.begin(), upperCmd.end(), upperCmd.begin(), ::toupper);

    try {
        if (upperCmd == "LET") {
            std::string varName;
            ss >> varName;
            std::transform(varName.begin(), varName.end(), varName.begin(), ::toupper);
            std::string remaining;
            std::getline(ss, remaining);
            size_t eqPos = remaining.find('=');
            if (eqPos == std::string::npos) throw std::runtime_error("SYNTAX ERROR");
            std::string expr = remaining.substr(eqPos + 1);
            auto tokens = tokenize(expr);
            ExpressionParser parser(tokens, scopeManager);
            scopeManager.setVariable(varName, parser.parse());
        } else if (upperCmd == "PRINT") {
            std::string expr;
            std::getline(ss, expr);
            auto tokens = tokenize(expr);
            ExpressionParser parser(tokens, scopeManager);
            std::cout << parser.parse() << std::endl;
        } else if (upperCmd == "INPUT") {
            std::string varName;
            ss >> varName;
            std::transform(varName.begin(), varName.end(), varName.begin(), ::toupper);
            long long val;
            if (!(std::cin >> val)) {
                std::cin.clear();
                std::string junk;
                std::getline(std::cin, junk);
            } else {
                scopeManager.setVariable(varName, val);
            }
        } else if (upperCmd == "INDENT") {
            scopeManager.indent();
        } else if (upperCmd == "DEDENT") {
            scopeManager.dedent();
        } else if (upperCmd == "RUN") {
            run();
        } else if (upperCmd == "LIST") {
            list();
        } else if (upperCmd == "CLEAR") {
            clear();
        } else if (upperCmd == "QUIT") {
            exit(0);
        } else if (upperCmd == "REM") {
            // Do nothing
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void Interpreter::clear() {
    program.clear();
    scopeManager.clear();
}

void Interpreter::list() {
    for (auto const& [lineNum, statement] : program) {
        std::cout << lineNum << " " << statement << std::endl;
    }
}

std::string Interpreter::trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \t\n\r");
    return s.substr(first, (last - first + 1));
}
