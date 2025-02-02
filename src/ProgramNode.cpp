#include "ProgramNode.hpp"
#include "instructions.hpp"

void ProgramNode::compile() {
    
    if (!procedures.empty()) {
        for (auto& proc : procedures) {
            proc->compile();
        }
    }

    main->compile();   
}