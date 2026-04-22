#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <string>
#include <vector>
#include <map>
#include "ScopeManager.hpp"
#include "ExpressionParser.hpp"

class Interpreter {
public:
    Interpreter();
    void processLine(const std::string& line);
    void run();
    void clear();
    void list();

private:
    std::map<int, std::string> program;
    ScopeManager scopeManager;
    bool running;

    void executeStatement(const std::string& statement);
    std::string trim(const std::string& s);
};

#endif
