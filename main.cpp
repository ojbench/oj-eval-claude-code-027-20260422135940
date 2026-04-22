#include "Interpreter.hpp"
#include <iostream>
#include <string>

int main() {
    Interpreter interpreter;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "QUIT") break;
        interpreter.processLine(line);
    }
    return 0;
}
