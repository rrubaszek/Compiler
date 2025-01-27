#include "ValueNode.hpp"
#include "instructions.hpp"

void ValueNode::compile() {
    switch (type) {
        case CONSTANT:
            program.emplace_back("SET", value);
            break;
        case OTHER :
            auto symbol = find_symbol(name);

            if (symbol->type == Type::VARIABLE) {
                program.emplace_back("LOAD", symbol->address);
                break;
            }

            if (symbol->type == Type::POINTER) {
                program.emplace_back("LOADI", symbol->address);
                break;
            }

            if (symbol->type == Type::ARRAY) {
                if (index_name != "") {
                    int a = symbol->address - symbol->start_address.value();
                    program.emplace_back("SET", a);
                    program.emplace_back("ADD", find_symbol(index_name)->address);
                    // int temp = allocate_register();
                    // program.emplace_back("STORE", temp);
                    // program.emplace_back("LOADI", temp);
                    // free_register(temp);
                }
                else {
                    int a = symbol->address - symbol->start_address.value() + index_value;
                    program.emplace_back("LOAD", a); 
                }
                break;
            }

            if (symbol->type == Type::ARRAY_POINTER) {
                if (index_name != "") {
                    program.emplace_back("SET", -symbol->start_address.value());
                    program.emplace_back("ADDI", symbol->address);
                    program.emplace_back("ADD", find_symbol(index_name)->address);
                    // int temp = allocate_register();
                    // program.emplace_back("STORE", temp);
                    // program.emplace_back("LOADI", temp);
                    // free_register(temp);
                }
                else {
                    program.emplace_back("SET", index_value - symbol->start_address.value()); 
                    program.emplace_back("ADDI", symbol->address); 
                }
                break;
            }
        // case VARIABLE:
        //     program.emplace_back("LOAD", find_symbol(name)->address);
        //     break;
        // case POINTER:
        //     program.emplace_back("LOADI", find_symbol(name)->address);
        //     break;
        // case ARRAY_ELEMENT:
        //     if (index_name != "") {
        //         int a = find_symbol(name)->zero_address.value();
        //         program.emplace_back("SET", a);
        //         program.emplace_back("ADD", find_symbol(index_name)->address);
        //         int temp = allocate_register();
        //         program.emplace_back("STORE", temp);
        //         program.emplace_back("LOADI", temp);
        //         free_register(temp);
        //     }
        //     else {
        //         int a = find_symbol(name)->zero_address.value() + index_value;
        //         program.emplace_back("LOAD", a); 
        //     }
        //     break;
    }
}