#include "ProgramNode.hpp"
#include "instructions.hpp"

ProgramNode::~ProgramNode() {
    if (!procedures.empty()) {
        for (auto& proc : procedures) {
            delete proc;
        }
    }
    delete main;
}

void ProgramNode::compile() {
    
    if (!procedures.empty()) {
        for (auto& proc : procedures) {
            proc->compile();
        }
    }

    main->compile();   
}

void ProgramNode::preprocessing() {
    if (!procedures.empty()) {
        for (auto& proc : procedures) {
            proc->preprocessing();
        }
    }

    main->preprocessing(); 
}