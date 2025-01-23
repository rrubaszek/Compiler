#include "DeclarationsNode.hpp"

void DeclarationsNode::compile() {
    for (auto& var : variables) {
        // Kompilacja deklaracji zmiennych
        std::cout << "Compiling variable declaration: " << var << "\n";
    }
}