#include "CommandNode.hpp"

void CommandNode::compile() {
    switch (type) {
        case ASSIGN: compile_assign(); break;
        case IF_ELSE: compile_if_else(); break;
        case IF: compile_if(); break;
        case WHILE: compile_while(); break;
        case REPEAT: compile_repeat(); break;
        case FOR: compile_for(); break;
        case FOR_REV: compile_for_rev(); break;
        case PROC_CALL: compile_proc_call(); break;
        case READ: compile_read(); break;
        case WRITE: compile_write(); break;
    }
}

void CommandNode::compile_assign() {
    std::cout << "Compile assign\n";
}

void CommandNode::compile_if_else() {
    std::cout << "Compile if else\n";
}

void CommandNode::compile_if() {
    std::cout << "Compile if\n";
}

void CommandNode::compile_while() {
    std::cout << "Compile while\n";
}

void CommandNode::compile_repeat() {
    std::cout << "Compile repeat\n";
}

void CommandNode::compile_for() {
    std::cout << "Compile for\n";
}

void CommandNode::compile_for_rev() {
    std::cout << "Compile for rev\n";
}

void CommandNode::compile_proc_call() {
    std::cout << "Compile proc call\n";
}

void CommandNode::compile_read() {
    std::cout << "Compile read\n";
}

void CommandNode::compile_write() {
    std::cout << "Compile write\n";
}