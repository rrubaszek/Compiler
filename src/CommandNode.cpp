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

    auto left = find_symbol(m_data.left->name);

    if (left->is_iterator) {
        yyerror("Iterator cannot be modified, line: ");
    }    

    if (!m_data.left->is_array) {
        if (left->type == Type::POINTER) {
            m_data.right->compile();
            program.emplace_back("STOREI", left->address); 
            return; 
        }
        
        if (left->type == Type::VARIABLE) {
            m_data.right->compile();
            program.emplace_back("STORE", left->address); 
            return; 
        }
    }

    if(m_data.left->is_array) {
        if (left->type == Type::ARRAY) {
            if (m_data.left->index_name != "") {
                auto index = find_symbol(m_data.left->index_name);

                program.emplace_back("SET", left->address - left->start_address.value());

                if (index->type == Type::POINTER) {
                    program.emplace_back("ADDI", index->address);
                }
                else {
                    program.emplace_back("ADD", index->address);
                }

                int temp = allocate_temp_register();
                program.emplace_back("STORE", temp);
                m_data.right->compile();
                program.emplace_back("STOREI", temp);
                free_temp_register(temp);
            }
            else {
                int a = left->address - left->start_address.value() + m_data.left->index_value;
                m_data.right->compile();
                program.emplace_back("STORE", a); 
            }
            return;
        }
        
        // TODO: fix arrays in procedures
        if (left->type == Type::ARRAY_POINTER) {
            if (m_data.left->index_name != "") {
                auto array_value = find_symbol(m_data.left->name + "_index");
                auto index = find_symbol(m_data.left->index_name);

                program.emplace_back("LOAD", left->address);
                program.emplace_back("SUBI", array_value->address); // Constains information of array start index (-10 eg)

                if (index->type == Type::POINTER) {
                    program.emplace_back("ADDI", index->address);
                }
                else {
                    program.emplace_back("ADD", index->address);
                }
                
                int temp = allocate_temp_register();
                program.emplace_back("STORE", temp);
                m_data.right->compile();
                program.emplace_back("STOREI", temp);
                free_temp_register(temp);
            }
            else {
                auto array_value = find_symbol(m_data.left->name + "_index");
                program.emplace_back("SET", m_data.left->index_value);
                program.emplace_back("ADDI", left->address);
                program.emplace_back("SUBI", array_value->address); // Constains information of array start index (-10 eg)

                int temp = allocate_temp_register();
                program.emplace_back("STORE", temp); 
                m_data.right->compile();
                program.emplace_back("STOREI", temp);
                free_temp_register(temp);
            }
            return;
        }
    }

    if (left->type == VARIABLE) {
        m_data.right->compile();
        program.emplace_back("STORE", left->address);  
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

    int startBody = program.size();
    m_data.loop_body->compile(); 

    m_data.condition->compile(); 
    program.emplace_back("JUMP", startBody - program.size());
}

void CommandNode::compile_for() {
    auto& m_data = std::get<ForData>(data);

    bool is_declared;
    auto symbol = find_symbol(m_data.loop_variable);
    if (symbol != nullptr) {
        symbol->is_iterator = true;
        is_declared = true;
    }
    else {
        add_symbol(m_data.loop_variable, allocate_register(), 1, is_local, true, VARIABLE, 1);
        is_declared = false;
    }

    symbol = find_symbol(m_data.loop_variable);

    m_data.start_value->compile();
    program.emplace_back("STORE", symbol->address);

    int loopBody = program.size();
    m_data.loop_body->compile();

    m_data.end_value->compile();
    program.emplace_back("SUB", symbol->address);
    program.emplace_back("JZERO", 5);

    program.emplace_back("SET", 1);
    program.emplace_back("ADD", symbol->address);
    program.emplace_back("STORE", symbol->address);

    program.emplace_back("JUMP", loopBody - program.size());

    // End scope
    if (!is_declared) {
        remove_symbol(m_data.loop_variable);
    }
}

void CommandNode::compile_for_rev() {
    auto& m_data = std::get<ForRevData>(data);

    // Create new scope
    bool is_declared;
    auto symbol = find_symbol(m_data.loop_variable);
    if (symbol != nullptr) {
        symbol->is_iterator = true;
        is_declared = true;
    }
    else {
        add_symbol(m_data.loop_variable, allocate_register(), 1, is_local, true, VARIABLE, 1);
        is_declared = false;
    }

    symbol = find_symbol(m_data.loop_variable);
    
    m_data.start_value->compile();
    program.emplace_back("STORE", symbol->address);

    int loopBody = program.size();
    m_data.loop_body->compile();

    m_data.end_value->compile();
    program.emplace_back("SUB", symbol->address);
    program.emplace_back("JZERO", 5);

    program.emplace_back("SET", -1);
    program.emplace_back("ADD", symbol->address);
    program.emplace_back("STORE", symbol->address);

    program.emplace_back("JUMP", loopBody - program.size());

    // End scope
    if (!is_declared) {
        remove_symbol(m_data.loop_variable);
    }
}

void CommandNode::compile_proc_call() {
    auto& m_data = std::get<ProcCallData>(data);

    std::cout << "Compiling procedure call: " << m_data.name << "\n";

    auto* proc = find_procedure(m_data.name);
    if (!proc) {
        yyerror("Undeclared procedure, line: ");
    }

    proc->is_called = true;

    int argIndex = proc->address;
    for (const auto& arg : m_data.args) {
        auto id = find_symbol(arg);

        if (id->type == Type::ARRAY) {
            // Additionally array needs to have a value of start_index -> put it in a register before array_start
            program.emplace_back("SET", id->start_address);
            auto temp = find_symbol(arg + "_index");
            program.emplace_back("STORE", temp->address);
            program.emplace_back("SET", temp->address);
            program.emplace_back("STORE", 1 + argIndex);

            argIndex++;

            program.emplace_back("SET", id->address);
            program.emplace_back("STORE", 1 + argIndex);
        } 
        else {
            program.emplace_back("SET", id->address);
            program.emplace_back("STORE", 1 + argIndex);
        }
        argIndex++;
    }

    // Procedure call
    program.emplace_back("SET", program.size() + 3);
    program.emplace_back("STORE", proc->address);
    program.emplace_back("JUMP", proc->relative_address - program.size());
}

void CommandNode::compile_read() {
    auto& m_data = std::get<ReadData>(data);

    auto symbol = find_symbol(m_data.target->name);

    if (symbol->type == Type::VARIABLE) {
        program.emplace_back("GET", symbol->address);  
        return;
    }

    if (symbol->type == Type::POINTER) {
        program.emplace_back("GET", 0);  
        program.emplace_back("STOREI", symbol->address);
        return;
    }

    if(symbol->type == Type::ARRAY) {
        if (m_data.target->index_name != "") {
            int a = symbol->address - symbol->start_address.value();
            auto index = find_symbol(m_data.target->index_name);

            program.emplace_back("SET", a);

            if (index->type == Type::POINTER) {
                program.emplace_back("ADDI", index->address);
            }
            else {
                program.emplace_back("ADD", index->address);
            }
            
            int temp = allocate_temp_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("GET", 0);
            program.emplace_back("STOREI", temp);
            free_temp_register(temp);
        }
        else { 
            int a = symbol->address - symbol->start_address.value() + m_data.target->index_value;

            if (a < symbol->start_address || a > symbol->size + std::abs(symbol->start_address.value())) {
                yyerror("Array out of bounds, line: ");
            }

            program.emplace_back("GET", a); 
        }
        return;
    }

    // TODO: fix arrays in procedures
    if(symbol->type == Type::ARRAY_POINTER) {
        if (m_data.target->index_name != "") {
            auto index = find_symbol(m_data.target->index_name);

            program.emplace_back("LOAD", symbol->address);
            program.emplace_back("SUBI", find_symbol(m_data.target->name + "_index")->address);

            if (index->type == Type::POINTER) {
                program.emplace_back("ADDI", index->address);
            }
            else {
                program.emplace_back("ADD", index->address);
            }
            
            
            int temp = allocate_temp_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("GET", 0);
            program.emplace_back("STOREI", temp);
            free_temp_register(temp);
        }
        else { 
            program.emplace_back("SET", m_data.target->index_value);
            program.emplace_back("ADD", symbol->address);
            program.emplace_back("SUBI", find_symbol(m_data.target->name + "_index")->address);

            int temp = allocate_temp_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("GET", 0);
            program.emplace_back("STOREI", temp);
            free_temp_register(temp);
        }
        return;
    }
}

void CommandNode::compile_write() {
    auto& m_data = std::get<WriteData>(data);

    if (m_data.value->type == ValueNode::ValueType::CONSTANT) {
        program.emplace_back("SET", m_data.value->value);
        program.emplace_back("PUT", 0);
        return;
    }

    auto symbol = find_symbol(m_data.value->name);

    if (symbol->type == Type::VARIABLE) {
        program.emplace_back("PUT", symbol->address);  
        return;
    }

    if (symbol->type == Type::POINTER) {
        program.emplace_back("LOADI", symbol->address);
        program.emplace_back("PUT", 0);  
        return;
    }

    if (symbol->type == Type::ARRAY) {
        if (m_data.value->index_name != "") {
            int a = symbol->address - symbol->start_address.value();
            auto index = find_symbol(m_data.value->index_name);

            program.emplace_back("SET", a);

            if (index->type == Type::POINTER) {
                program.emplace_back("ADDI", index->address);
            }
            else {
                program.emplace_back("ADD", index->address);
            }
            
            int temp = allocate_temp_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("LOADI", temp);
            program.emplace_back("PUT", 0);
            free_temp_register(temp);
        }
        else { 
            int a = symbol->address - symbol->start_address.value() + m_data.value->index_value;

            if (a < symbol->start_address || a > symbol->size + std::abs(symbol->start_address.value())) {
                yyerror("Array out of bounds, line: ");
            }

            program.emplace_back("PUT", a); 
        }
    }

    // TODO: fix arrays in procedures
    if (symbol->type == Type::ARRAY_POINTER) {
        if (m_data.value->index_name != "") {
            auto index = find_symbol(m_data.value->index_name);

            program.emplace_back("LOAD", symbol->address);

            if (index->type == Type::POINTER) {
                program.emplace_back("ADDI", index->address);
            }
            else {
                program.emplace_back("ADD", index->address);
            }

            program.emplace_back("SUBI", find_symbol(m_data.value->name + "_index")->address);
            
            int temp = allocate_temp_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("LOADI", temp);
            program.emplace_back("PUT", 0);
            free_temp_register(temp);
        }
        else { 
            program.emplace_back("SET", m_data.value->index_value);
            program.emplace_back("ADD", symbol->address);
            program.emplace_back("SUBI", find_symbol(m_data.value->name + "_index")->address);

            int temp = allocate_temp_register();
            program.emplace_back("STORE", temp);
            program.emplace_back("LOADI", temp);
            program.emplace_back("PUT", 0);
            free_temp_register(temp);
        }
    }
}