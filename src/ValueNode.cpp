#include "ValueNode.hpp"
#include "instructions.hpp"

void ValueNode::compile() {
    switch (type) {
        case CONSTANT:
            program.emplace_back("SET ", value);
            break;
        case VARIABLE:
            program.emplace_back("LOAD ", find_symbol(name)->address);
            break;
        case ARRAY_ELEMENT:
            if (index_name != "") {
                int a = find_symbol(name)->zero_address;
                program.emplace_back("SET", a);
                program.emplace_back("ADD", find_symbol(index_name)->address);
                int temp = allocate_register();
                program.emplace_back("STORE", temp);
                program.emplace_back("LOADI", temp);
                free_register(temp);
            }
            else {
                int a = find_symbol(name)->zero_address + index_value;
                program.emplace_back("LOAD", a); 
            }
            break;
    }
}