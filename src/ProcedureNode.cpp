#include "ProcedureNode.hpp"
#include "instructions.hpp"

void ProcedureNode::compile() {
    // Kod kompilacji dla procedury
    std::cout << "Compiling procedure: " << name << "\n";
    is_local = true;
    local_symbol_stack.push({});

    for (const auto& arg : args) {
        if (arg.second) {
            std::cout << "array " << arg.first << "\n"; // Tablica
        } else {
            std::cout << "variable " << arg.first << "\n"; // Zmienna
        }
    }

    if (declarations != nullptr) {
        declarations->compile();  
    }

    if (commands != nullptr) {
        commands->compile();
    }

    local_symbol_stack.pop();
    is_local = false;
}