#include "DeclarationsNode.hpp"
#include "instructions.hpp"
#include "instructions.hpp"

void DeclarationsNode::compile() {
    for (const auto& var : variables) {
        if (find_symbol(var)) {
            throw_error("zmienna o takiej nazwie została już zadeklarowana, linia: ", lineno);
            continue;
        }
        add_symbol(var, allocate_register(), std::nullopt, is_local, false, true, VARIABLE, 1);
    }

    for (const auto& array : arrays) {
        if (find_symbol(array.name)) {
            throw_error("tablica o takiej nazwie została już zadeklarowana, linia: ", lineno);
            continue;
        }

        add_symbol(array.name + "_index", allocate_register(), std::nullopt, is_local, false, false, VARIABLE, 1);

        int array_start = allocate_register();
        int array_size = array.end - array.start + 1;
        add_symbol(array.name, array_start, array.start, is_local, false, false, ARRAY, array_size);
        next_free_register += array_size;
    }
}