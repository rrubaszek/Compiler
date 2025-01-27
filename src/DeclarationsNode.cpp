#include "DeclarationsNode.hpp"
#include "instructions.hpp"
#include "instructions.hpp"

void DeclarationsNode::compile() {
    for (const auto& var : variables) {
        if (find_symbol(var)) {
            std::cerr << "Variable already declared: " << var << "\n";
            continue;
        }
        add_symbol(var, allocate_register(), std::nullopt, is_local, false, VARIABLE);
    }

    for (const auto& array : arrays) {
        if (find_symbol(array.name)) {
            yyerror("Array with that name has already been declared, line: ");
            continue;
        }
        int array_start = allocate_register();
        int array_size = array.end - array.start + 1;
        add_symbol(array.name, array_start, array.start, is_local, false, ARRAY);
        next_free_register += array_size;
        std::cout << "Allocated array " << array.name << " " << array.start << " " << array.end << " " << next_free_register << "\n";
    }
}