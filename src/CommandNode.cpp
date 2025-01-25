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

    if (find_symbol(m_data.left->name)->is_iterator) {
        std::cout << "Iterator cannot be modified\n";
        exit(1);
    }

    if (!m_data.left->is_array) {
        m_data.right->compile();
        program.emplace_back("STORE", find_symbol(m_data.left->name)->address);  
        return;
    }

    if(m_data.left->is_array) {
        if (m_data.left->index_name != "") {
            int a = find_symbol(m_data.left->name)->zero_address;
            program.emplace_back("SET", a);
            program.emplace_back("ADD", find_symbol(m_data.left->index_name)->address);
            int temp = allocate_register();
            program.emplace_back("STORE", temp);
            m_data.right->compile();
            program.emplace_back("STOREI", temp);
            free_register(temp);
        }
        else {
            int a = find_symbol(m_data.left->name)->zero_address + m_data.left->index_value;
            m_data.right->compile();
            program.emplace_back("STORE", a); 
        }
    }
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
    auto& m_data = std::get<WhileData>(data);

    int condIndex = program.size();
    m_data.condition->compile(); 
    int jumpToEndWhile = program.size();
    program.emplace_back("JUMP", -1);

    m_data.loop_body->compile(); 
    program.emplace_back("JUMP", condIndex - program.size());
    int endWhile = program.size();
    program[jumpToEndWhile].operand = endWhile - jumpToEndWhile;
}

void CommandNode::compile_repeat() {
    auto& m_data = std::get<RepeatData>(data);
    if (m_data.condition->type == ConditionNode::ConditionType::GT)
        m_data.condition->type = ConditionNode::ConditionType::LEQ; 
    else if (m_data.condition->type == ConditionNode::ConditionType::LT)
        m_data.condition->type = ConditionNode::ConditionType::GEQ;
    else if (m_data.condition->type == ConditionNode::ConditionType::EQ)
        m_data.condition->type = ConditionNode::ConditionType::NEQ;        
    else if (m_data.condition->type == ConditionNode::ConditionType::NEQ)
        m_data.condition->type = ConditionNode::ConditionType::EQ;
    else if (m_data.condition->type == ConditionNode::ConditionType::LEQ)
        m_data.condition->type = ConditionNode::ConditionType::GT;
    else if (m_data.condition->type == ConditionNode::ConditionType::GEQ)
        m_data.condition->type = ConditionNode::ConditionType::LT;

    int startBody = program.size();
    m_data.loop_body->compile(); 

    m_data.condition->compile(); 
    program.emplace_back("JUMP", 2);
    program.emplace_back("JUMP", startBody - program.size());
}

void CommandNode::compile_for() {
    auto& m_data = std::get<ForData>(data);

    // Create new scope
    local_symbol_stack.push({});
    add_symbol(m_data.loop_variable, allocate_register(), 1, true, true);
    
    m_data.start_value->compile();
    program.emplace_back("STORE", find_symbol(m_data.loop_variable)->address);

    int loopBody = program.size();
    m_data.loop_body->compile();

    m_data.end_value->compile();
    program.emplace_back("SUB", find_symbol(m_data.loop_variable)->address);
    program.emplace_back("JZERO", 5);

    program.emplace_back("SET", 1);
    program.emplace_back("ADD", find_symbol(m_data.loop_variable)->address);
    program.emplace_back("STORE", find_symbol(m_data.loop_variable)->address);

    program.emplace_back("JUMP", loopBody - program.size());

    // End scope
    remove_symbol(m_data.loop_variable);
    local_symbol_stack.pop();
}

void CommandNode::compile_for_rev() {
    auto& m_data = std::get<ForRevData>(data);

    // Create new scope
    local_symbol_stack.push({});
    add_symbol(m_data.loop_variable, allocate_register(), 1, true, true);
    
    m_data.start_value->compile();
    program.emplace_back("STORE", find_symbol(m_data.loop_variable)->address);

    int loopBody = program.size();
    m_data.loop_body->compile();

    m_data.end_value->compile();
    program.emplace_back("SUB", find_symbol(m_data.loop_variable)->address);
    program.emplace_back("JZERO", 5);

    program.emplace_back("SET", -1);
    program.emplace_back("ADD", find_symbol(m_data.loop_variable)->address);
    program.emplace_back("STORE", find_symbol(m_data.loop_variable)->address);

    program.emplace_back("JUMP", loopBody - program.size());

    // End scope
    remove_symbol(m_data.loop_variable);
    local_symbol_stack.pop();
}

void CommandNode::compile_proc_call() {
    std::cout << "Compile proc call\n";
}

void CommandNode::compile_read() {
    auto& m_data = std::get<ReadData>(data);
    if (!m_data.target->is_array) {
        program.emplace_back("GET", find_symbol(m_data.target->name)->address);  
        return;
    }

    if(m_data.target->is_array) {
        if (m_data.target->index_name != "") {
            int a = find_symbol(m_data.target->name)->zero_address;
            program.emplace_back("SET", a);
            program.emplace_back("ADD", find_symbol(m_data.target->index_name)->address);
            
            int temp = allocate_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("GET", 0);
            program.emplace_back("STOREI", temp);
            free_register(temp);
        }
        else { 
            int a = find_symbol(m_data.target->name)->zero_address + m_data.target->index_value;
            program.emplace_back("GET", a); 
        }
    }
}

void CommandNode::compile_write() {
    auto& m_data = std::get<WriteData>(data);
    if (m_data.value->type == ValueNode::ValueType::VARIABLE) {
        program.emplace_back("PUT", find_symbol(m_data.value->name)->address);  
        return;
    }

    if(m_data.value->type == ValueNode::ValueType::ARRAY_ELEMENT) {
        if (m_data.value->index_name != "") {
            int a = find_symbol(m_data.value->name)->zero_address;
            program.emplace_back("SET", a);
            program.emplace_back("ADD", find_symbol(m_data.value->index_name)->address);
            program.emplace_back("PUT", 0);
        }
        else { 
            int a = find_symbol(m_data.value->name)->zero_address + m_data.value->index_value;
            program.emplace_back("PUT", a); 
        }
    }
}