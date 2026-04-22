#ifndef SCOPE_MANAGER_HPP
#define SCOPE_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

class ScopeManager {
public:
    ScopeManager() {
        // Initial global scope
        scopes.push_back({});
    }

    void indent() {
        scopes.push_back({});
    }

    void dedent() {
        if (scopes.size() <= 1) {
            throw std::runtime_error("SCOPE UNDERFLOW");
        }
        scopes.pop_back();
    }

    void setVariable(const std::string& name, long long value) {
        // Shadowing: always set in the current (innermost) scope
        scopes.back()[name] = value;
    }

    long long getVariable(const std::string& name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return found->second;
            }
        }
        throw std::runtime_error("VARIABLE NOT DEFINED");
    }

    void clear() {
        scopes.clear();
        scopes.push_back({});
    }

private:
    std::vector<std::map<std::string, long long>> scopes;
};

#endif
