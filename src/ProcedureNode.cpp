#include "ProcedureNode.hpp"
#include "instructions.hpp"

void ProcedureNode::compile() {
    if (first_pass) {
        int relative_address = program.size() + 1;
        ll proc_register = allocate_register();
        add_procedure(name, proc_register, relative_address, false, args); // Set is_called to false, if later encountered then change to true
        called_procedures.push(name);
    }

    auto proc = find_procedure(name);
    if (proc == nullptr) {
        return; // Procedure has been removed in preprocessing
    }
    else {
        // Here you need to fix procedure parameter after removing other
        auto proc = find_procedure(name);
        proc->address = allocate_register();
        proc->relative_address = program.size() + 1;
    }
    
    is_local = true;
    local_symbol_stack.push({});

    for (const auto& arg : args) {
        if (arg.second) {
            // Array, pass pointer to 0 element in T, not an entire array, so basically type is also pointer
            add_symbol(arg.first + "_index", allocate_register(), std::nullopt, is_local, false, false, VARIABLE, 1);
            add_symbol(arg.first, allocate_register(), std::nullopt, is_local, false, false, ARRAY_POINTER, 1);
        } 
        else {
            add_symbol(arg.first, allocate_register(), std::nullopt, is_local, false, false, POINTER, 1);
        }
    }

    int start = program.size();
    // Placeholder to skip procedure
    int procedureStart = program.size();
    program.emplace_back("JUMP", -1);

    if (declarations != nullptr) {
        declarations->compile();  
    }

    if (commands != nullptr) {
        commands->compile();
    }

    program.emplace_back("RTRN", find_procedure(name)->address);
    program[procedureStart].operand = program.size() - start;

    local_symbol_stack.pop();
    is_local = false;
}