#include "ProcedureNode.hpp"
#include "instructions.hpp"

void ProcedureNode::compile() {
    std::cout << "Compiling procedure: " << name << "\n";

    add_procedure(name, allocate_register(), program.size() + 1, false); // Set is_called to false, if later encountered then change to true

    is_local = true;
    local_symbol_stack.push({});

    for (const auto& arg : args) {
        if (arg.second) {
            // Array, pass pointer to 0 element in T
            add_symbol(arg.first, allocate_register(), 1, is_local, false);
        } else {
            add_symbol(arg.first, allocate_register(), 1, is_local, false);
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
    program[procedureStart].operand = program.size();

    local_symbol_stack.pop();
    is_local = false;
}