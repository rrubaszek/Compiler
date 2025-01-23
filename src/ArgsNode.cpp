#include "ArgsNode.hpp"

void ArgsNode::compile() {
    for (auto& arg : args) {
        // Kompilacja argumentów procedury
        std::cout << "Compiling procedure argument: " << arg << "\n";
    }
}