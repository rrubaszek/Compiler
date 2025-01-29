#include "DeclarationsNode.hpp"
#include "instructions.hpp"
#include "instructions.hpp"

void DeclarationsNode::compile() {
    for (const auto& var : variables) {
        if (find_symbol(var)) {
            std::cerr << "Variable already declared: " << var << "\n";
            continue;
        }
        add_symbol(var, allocate_register(), std::nullopt, is_local, false, VARIABLE, 1);
    }

    for (const auto& array : arrays) {
        if (find_symbol(array.name)) {
            yyerror("Array with that name has already been declared, line: ");
            continue;
        }

        add_symbol(array.name + "_index", allocate_register(), std::nullopt, is_local, false, VARIABLE, 1);
        std::cout << "Allocated array, pre_value: " << next_free_register - 1 << "\n";

        int array_start = allocate_register();
        int array_size = array.end - array.start + 1; // TODO: fix arrays in procedures
        add_symbol(array.name, array_start, array.start, is_local, false, ARRAY, array_size);
        next_free_register += array_size;
        std::cout << "Allocated array, reg: " << array_start << " " << array.name << " [" << array.start << ":" << array.end << "] " << next_free_register << "\n";

        // TODO: fix arrays in procedures
        // First address in array holds inforamtion about start value [-10:10] -> -10
        // program.emplace_back("SET", array.start);
        // program.emplace_back("STORE", array_start);
    }
}