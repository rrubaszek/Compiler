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
                    auto index = find_symbol(index_name);

                    program.emplace_back("LOAD", symbol->address);
                    program.emplace_back("SUBI", find_symbol(name + "_index")->address);

                    if (index->type == Type::POINTER) {
                        program.emplace_back("ADDI", index->address);
                    }
                    else {  
                        program.emplace_back("ADD", index->address);
                    }
                    
                    int temp = allocate_temp_register();
                    program.emplace_back("STORE", temp);
                    program.emplace_back("LOADI", temp);
                    free_temp_register(temp);
                }
                else {
                    int a = symbol->address - symbol->start_address.value() + index_value;
                    program.emplace_back("LOAD", a); 
                }
                break;
            }

            // TODO: fix arrays in procedures, bc array_pointer does not have start_address
            if (symbol->type == Type::ARRAY_POINTER) {
                if (index_name != "") {
                    auto index = find_symbol(index_name);

                    program.emplace_back("LOAD", symbol->address);
                    program.emplace_back("SUBI", find_symbol(name + "_index")->address);

                    if (index->type == Type::POINTER) {
                        program.emplace_back("ADDI", index->address);
                    }
                    else {
                        program.emplace_back("ADD", index->address); 
                    }
                    
                    int temp = allocate_temp_register();
                    program.emplace_back("STORE", temp);
                    program.emplace_back("LOADI", temp);
                    free_temp_register(temp);
                }
                else {
                    program.emplace_back("SET", index_value); 
                    program.emplace_back("ADD", symbol->address); 
                    program.emplace_back("SUBI", find_symbol(name + "_index")->address);
                    int temp = allocate_temp_register();
                    program.emplace_back("STORE", temp);
                    program.emplace_back("LOADI", temp);
                    free_temp_register(temp);
                }
                break;
            }
    }
}