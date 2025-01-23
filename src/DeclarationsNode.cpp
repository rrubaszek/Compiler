#include "DeclarationsNode.hpp"
#include "instructions.hpp"
#include "instructions.hpp"

void DeclarationsNode::compile() {
    for (const auto& var : variables) {
        if (find_symbol(var)) {
            std::cerr << "Variable already declared: " << var << "\n";
            continue;
        }
        add_symbol(var, allocate_register(), 1, is_local);
    }

    for (const auto& array : arrays) {
        if (find_symbol(array.name)) {
            std::cerr << "Array already declared: " << array.name << "\n";
            continue;
        }
        int array_size = array.end - array.start + 1;
        add_symbol(array.name, allocate_register(), array_size, is_local);
        next_free_register += array_size;
    }
}