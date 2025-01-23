#include "CommandNode.hpp"
#include "CommandsNode.hpp"
#include "instructions.hpp"

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
    auto& m_data = std::get<AssignData>(data);
    m_data.right->compile(); 
    program.emplace_back("STORE", find_symbol(m_data.left->name)->address);  
}

void CommandNode::compile_if_else() {
    auto& m_data = std::get<IfElseData>(data);

    m_data.condition->compile(); 
    int jumpToElseIndex = program.size();
    program.emplace_back("JUMP", -1);

    m_data.then_branch->compile();  // Kompilacja bloku THEN
    int jumpToEndIfIndex = program.size();
    program.emplace_back("JUMP", -1);

    int elseIndex = program.size();
    program[jumpToElseIndex].operand = elseIndex - jumpToElseIndex;

    m_data.else_branch->compile();  // Kompilacja bloku ELSE
    int endIfIndex = program.size();
    program[jumpToEndIfIndex].operand = endIfIndex - jumpToEndIfIndex;
}

void CommandNode::compile_if() {
    auto& m_data = std::get<IfData>(data);

    m_data.condition->compile(); 
    int jumpToEndIfIndex = program.size();
    program.emplace_back("JUMP", -1);

    m_data.then_branch->compile(); 
    int endIfIndex = program.size();
    program[jumpToEndIfIndex].operand = endIfIndex - jumpToEndIfIndex;
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
    auto& m_data = std::get<ReadData>(data);
    program.emplace_back("GET", find_symbol(m_data.target->name)->address);
}

void CommandNode::compile_write() {
    auto& m_data = std::get<WriteData>(data);
    program.emplace_back("PUT", find_symbol(m_data.value->name)->address);
}