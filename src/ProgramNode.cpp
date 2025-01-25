#include "ProgramNode.hpp"
#include "instructions.hpp"

#include <algorithm>

void ProgramNode::compile() {
    // Kod kompilacji dla programu
    if (!procedures.empty()) {
        for (auto& proc : procedures) {
            proc->compile();
        }
    }

    main->compile();

    for (auto it = procedure_table.begin(); it != procedure_table.end(); ) {
    auto& proc = it->second;

    if (!proc.is_called) {
        auto procedureStart = program.begin() + proc.relative_address - 1;

        auto procedureEnd = std::find_if(
            procedureStart, program.end(),
            [](const instruction& instr) {
                return instr.opcode == "RTRN";
            }
        );

        if (procedureEnd != program.end()) {
            program.erase(procedureStart, procedureEnd + 1);
        }

        it = procedure_table.erase(it); 
    } 
    else {
        ++it; 
    }
}

}