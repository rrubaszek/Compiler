#include "ArgsNode.hpp"
#include "instructions.hpp"

void ArgsNode::compile() {
    for (auto& arg : args) {
        // Kompilacja argumentów procedury
        std::cout << "Compiling procedure argument: " << arg << "\n";
    }
}