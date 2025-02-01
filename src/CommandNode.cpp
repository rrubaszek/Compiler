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

    if (left == nullptr) {
        throw_error("niezadeklarowana zmienna, linia: ", lineno);
    }

    left->is_uninitialized = false;
    
    if (left->is_iterator) {
        throw_error("próba modyfikacji iteratora pętli, linia: ", lineno);
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

        throw_error("niewłaściwe użycie zmiennej " + m_data.left->name + ", linia: ", lineno);

    }
    
    if(m_data.left->is_array) {
        if (left->type == Type::ARRAY) {
            if (m_data.left->index_name != "") {
                auto index = find_symbol(m_data.left->index_name);

                if (index == nullptr) {
                    throw_error("niezadeklarowana zmienna, linia: ", lineno);
                }
                if (index->is_uninitialized) {
                    throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                }

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
        
        if (left->type == Type::ARRAY_POINTER) {
            if (m_data.left->index_name != "") {
                auto array_value = find_symbol(m_data.left->name + "_index");
                auto index = find_symbol(m_data.left->index_name);

                if (index == nullptr) {
                    throw_error("niezadeklarowana zmienna, linia: ", lineno);
                }
                if (index->is_uninitialized) {
                    throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                }

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

        throw_error("niewłaściwe użycie tablicy " + m_data.left->name + ", linia: ", lineno);
    }
}

void CommandNode::compile_if_else() {
    auto& m_data = std::get<IfElseData>(data);

    m_data.condition->compile(); 
    if (m_data.condition->res == ConditionNode::ConditionResult::TRUE) {
        m_data.then_branch->compile();
        return;
    }
    
    if (m_data.condition->res == ConditionNode::ConditionResult::FALSE) {
        m_data.else_branch->compile();
        return;
    }

    int jumpToElseIndex = program.size();
    program.emplace_back("JUMP", -1);

    m_data.then_branch->compile();  // Kompilacja bloku THEN
    int jumpToEndIfIndex = program.size();
    program.emplace_back("JUMP", -1);

    program[jumpToElseIndex].operand = program.size() - jumpToElseIndex;

    m_data.else_branch->compile();  // Kompilacja bloku ELSE

    program[jumpToEndIfIndex].operand =  program.size() - jumpToEndIfIndex;
}

void CommandNode::compile_if() {
    auto& m_data = std::get<IfData>(data);

    m_data.condition->compile(); 

    if (m_data.condition->res == ConditionNode::ConditionResult::FALSE) {
        return;
    }

    if (m_data.condition->res == ConditionNode::ConditionResult::TRUE) {
        m_data.then_branch->compile(); 
        return;
    }

    int jumpToEndIfIndex = program.size();
    program.emplace_back("JUMP", -1);

    m_data.then_branch->compile(); 

    program[jumpToEndIfIndex].operand = program.size() - jumpToEndIfIndex;
}

void CommandNode::compile_while() {
    auto& m_data = std::get<WhileData>(data);

    int condIndex = program.size();
    m_data.condition->compile(); 

    if (m_data.condition->res == ConditionNode::ConditionResult::FALSE) {
        return;
    }

    if (m_data.condition->res == ConditionNode::ConditionResult::TRUE) {
        m_data.loop_body->compile(); 
        program.emplace_back("JUMP", condIndex - program.size());
        throw_error("nieskończona pętla, linia: ", lineno);
        return;
    }

    int jumpToEndWhile = program.size();
    program.emplace_back("JUMP", -1);

    m_data.loop_body->compile(); 
    program.emplace_back("JUMP", condIndex - program.size());

    program[jumpToEndWhile].operand = program.size() - jumpToEndWhile;
}

void CommandNode::compile_repeat() {
    auto& m_data = std::get<RepeatData>(data);

    int startBody = program.size();
    m_data.loop_body->compile(); 

    m_data.condition->compile(); 

    if (m_data.condition->res == ConditionNode::ConditionResult::FALSE) {
        program.emplace_back("JUMP", startBody- program.size());
        throw_error("nieskończona pętla, linia: ", lineno);
        return;
    }

    if (m_data.condition->res == ConditionNode::ConditionResult::TRUE) {
        return;
    }

    program.emplace_back("JUMP", startBody - program.size());
}

void CommandNode::compile_for() {
    auto& m_data = std::get<ForData>(data);

    bool is_declared;
    auto symbol = find_symbol(m_data.loop_variable);
    if (symbol != nullptr) {
        symbol->is_iterator = true;
        symbol->is_uninitialized = false;
        is_declared = true;
    }
    else {
        add_symbol(m_data.loop_variable, allocate_register(), 1, is_local, true, false, VARIABLE, 1);
        is_declared = false;
    }

    symbol = find_symbol(m_data.loop_variable);

    m_data.start_value->compile();
    program.emplace_back("STORE", symbol->address);

    int loopBody = program.size();
    m_data.end_value->compile();
    program.emplace_back("SUB", symbol->address);
    int condition = program.size();
    program.emplace_back("JZERO", -1);

    m_data.loop_body->compile();

    program.emplace_back("SET", 1);
    program.emplace_back("ADD", symbol->address);
    program.emplace_back("STORE", symbol->address);

    program.emplace_back("JUMP", loopBody - program.size());
    program[condition].operand = program.size() - condition;

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
        symbol->is_uninitialized = false;
        is_declared = true;
    }
    else {
        add_symbol(m_data.loop_variable, allocate_register(), 1, is_local, true, false, VARIABLE, 1);
        is_declared = false;
    }

    symbol = find_symbol(m_data.loop_variable);
    
    m_data.start_value->compile();
    program.emplace_back("STORE", symbol->address);

    int loopBody = program.size();
    m_data.end_value->compile();
    program.emplace_back("SUB", symbol->address);
    int condition = program.size();
    program.emplace_back("JZERO", -1);

    m_data.loop_body->compile();

    program.emplace_back("SET", -1);
    program.emplace_back("ADD", symbol->address);
    program.emplace_back("STORE", symbol->address);

    program.emplace_back("JUMP", loopBody - program.size());
    program[condition].operand = program.size() - condition;

    // End scope
    if (!is_declared) {
        remove_symbol(m_data.loop_variable);
    }
}

void CommandNode::compile_proc_call() {
    auto& m_data = std::get<ProcCallData>(data);

    auto proc = find_procedure(m_data.name);

    if (proc == nullptr) {
        throw_error("niezadeklarowana procedura " + m_data.name + ", linia: ", lineno);
    }

    proc->is_called = true;

    int current_arg = 0;
    int argIndex = proc->address;
    for (const auto& arg : m_data.args) {
        auto id = find_symbol(arg);

        if (id == nullptr) {
            throw_error("niezadeklarowana zmienna, linia: ", lineno);
        }

        id->is_uninitialized = false;

        if (id->type == Type::ARRAY && proc->args[current_arg].second == true) {
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
        else if (id->type == Type::POINTER && proc->args[current_arg].second == false) {
            program.emplace_back("LOAD", id->address);
            program.emplace_back("STORE", 1 + argIndex);
        }
        else if (id->type == Type::ARRAY_POINTER && proc->args[current_arg].second == true) {
            // TODO: test this, seems to be working fine
            program.emplace_back("LOAD", find_symbol(arg + "_index")->address);
            program.emplace_back("STORE", 1 + argIndex);

            argIndex++;

            program.emplace_back("LOAD", id->address);
            program.emplace_back("STORE", 1 + argIndex);
        }
        else if (id->type == Type::VARIABLE && proc->args[current_arg].second == false) {
            program.emplace_back("SET", id->address);
            program.emplace_back("STORE", 1 + argIndex);
        }
        else {
            throw_error("niewłaściwy parametr procedury " + m_data.name + ", linia: ", lineno);
        }
        argIndex++;
        current_arg++;
    }

    // Procedure call
    program.emplace_back("SET", program.size() + 3);
    program.emplace_back("STORE", proc->address);
    program.emplace_back("JUMP", proc->relative_address - program.size());
}

void CommandNode::compile_read() {
    auto& m_data = std::get<ReadData>(data);

    auto symbol = find_symbol(m_data.target->name);

    if (symbol == nullptr) {
        throw_error("niezadeklarowana zmienna, linia: ", lineno);
    }

    symbol->is_uninitialized = false;

    if (!m_data.target->is_array) {
        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("GET", symbol->address);  
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("GET", 0);  
            program.emplace_back("STOREI", symbol->address);
            return;
        }

        throw_error("nieprawidłowe użycie zmiennej, linia: ", lineno);
    }

    if (m_data.target->is_array) {
        if(symbol->type == Type::ARRAY) {
            if (m_data.target->index_name != "") {
                int a = symbol->address - symbol->start_address.value();
                auto index = find_symbol(m_data.target->index_name);

                if (index == nullptr) {
                    throw_error("niezadeklarowana zmienna, linia: ", lineno);
                }
                if (index->is_uninitialized) {
                    throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                }

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
                    throw_error("indeks poza zakresem tablicy, linia: ", lineno);
                }

                program.emplace_back("GET", a); 
            }
            return;
        }

        if(symbol->type == Type::ARRAY_POINTER) {
            if (m_data.target->index_name != "") {
                auto index = find_symbol(m_data.target->index_name);

                if (index == nullptr) {
                    throw_error("niezadeklarowana zmienna, linia: ", lineno);
                }
                if (index->is_uninitialized) {
                    throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                }               

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
        throw_error("nieprawidłowe użycie tablicy, linia: ", lineno);
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

    if (symbol == nullptr) {
        throw_error("niezadeklarowana zmienna, linia: ", lineno);
    }
    if (symbol->is_uninitialized) {
        throw_error("niezainicjalizowana zmienna, linia: ", lineno);
    }

    if (!m_data.value->is_array) {
        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("PUT", symbol->address);  
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("LOADI", symbol->address);
            program.emplace_back("PUT", 0);  
            return;
        }

        throw_error("nieprawidłowe użycie zmiennej, linia: ", lineno);
    }

    if (m_data.value->is_array) {
        if (symbol->type == Type::ARRAY) {
            if (m_data.value->index_name != "") {
                int a = symbol->address - symbol->start_address.value();
                auto index = find_symbol(m_data.value->index_name);

                if (index == nullptr) {
                    throw_error("niezadeklarowana zmienna, linia: ", lineno);
                }
                if (index->is_uninitialized) {
                    throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                }

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
                    throw_error("indeks poza zakresem tablicy, linia: ", lineno);
                }

                program.emplace_back("PUT", a); 
            }
            return;
        }

        if (symbol->type == Type::ARRAY_POINTER) {
            if (m_data.value->index_name != "") {
                auto index = find_symbol(m_data.value->index_name);

                if (index == nullptr) {
                    throw_error("niezadeklarowana zmienna, linia: ", lineno);
                }
                if (index->is_uninitialized) {
                    throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                }

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
            return;
        }

        throw_error("nieprawidłowe użycie tablicy, linia: ", lineno);
    }
}