#include "ProcedureNode.hpp"
#include "instructions.hpp"

void ProcedureNode::compile() {
    int start = program.size();
    std::cout << "Compiling procedure: " << name << "\n";

    add_procedure(name, allocate_register(), program.size() + 1, false); // Set is_called to false, if later encountered then change to true

    is_local = true;
    local_symbol_stack.push({});

    for (const auto& arg : args) {
        if (arg.second) {
            // Array, pass pointer to 0 element in T, not an entire array, so basically type is also pointer
            add_symbol(arg.first + "_index", allocate_register(), std::nullopt, is_local, false, VARIABLE, 1);
            add_symbol(arg.first, allocate_register(), std::nullopt, is_local, false, ARRAY_POINTER, 1);
            std::cout << "ARRAY POINTER\n";
        } 
        else {
            add_symbol(arg.first, allocate_register(), std::nullopt, is_local, false, POINTER, 1);
        }
    }

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