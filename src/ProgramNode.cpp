#include "ProgramNode.hpp"
#include "instructions.hpp"

void ProgramNode::compile() {
    // Kod kompilacji dla programu
    if (!procedures.empty()) {
        for (auto& proc : procedures) {
            proc->compile();
        }
    }
    main->compile();
}