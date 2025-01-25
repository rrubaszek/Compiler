#include "DeclarationsNode.hpp"
#include "instructions.hpp"
#include "instructions.hpp"

void DeclarationsNode::compile() {
    for (const auto& var : variables) {
        if (find_symbol(var)) {
            std::cerr << "Variable already declared: " << var << "\n";
            continue;
        }
        add_symbol(var, allocate_register(), 1, is_local, false);
    }

    for (const auto& array : arrays) {
        if (find_symbol(array.name)) {
            std::cerr << "Array already declared: " << array.name << "\n";
            continue;
        }
        int array_start = allocate_register();
        int array_size = array.end - array.start + 1;
        int zero_index = array_start + std::abs(array.start);
        add_symbol(array.name, array_start, zero_index, is_local, false);
        next_free_register += array_size;
        std::cout << "Allocated array " << array.name << " " << array.start << " " << array.end << " " << next_free_register << "\n";
    }
}