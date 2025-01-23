#include "ProcedureNode.hpp"

void ProcedureNode::compile() {
    // Kod kompilacji dla procedury
    std::cout << "Compiling procedure: " << name << "\n";
    declarations->compile();
    commands->compile();
}