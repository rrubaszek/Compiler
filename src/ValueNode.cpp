#include "ValueNode.hpp"
#include "instructions.hpp"

void ValueNode::compile() {
    switch (type) {
        case CONSTANT:
            program.emplace_back("SET", value);
            break;
        case OTHER :
            auto symbol = find_symbol(name);

            if (symbol == nullptr) {
                throw_error("niezadeklarowana zmienna, linia: ", lineno);
            }
            if (symbol->is_uninitialized) {
                throw_error("niezainicjalizowana zmienna, linia: ", lineno);
            }

            if (!is_array) {
                if (symbol->type == Type::VARIABLE) {
                    program.emplace_back("LOAD", symbol->address);
                    break;
                }

                if (symbol->type == Type::POINTER) {
                    program.emplace_back("LOADI", symbol->address);
                    break;
                }

                throw_error("niewłaściwe użycie zmiennej, linia: ", lineno);
            }

            if (is_array) {
                if (symbol->type == Type::ARRAY) {
                    if (index_name != "") {
                        auto index = find_symbol(index_name);

                        if (index == nullptr) {
                            throw_error("niezadeklarowana zmienna, linia: ", lineno);
                        }
                        if (index->is_uninitialized) {
                            throw_error("niezainicjalizowana zmienna, linia: ", lineno);
                        }

                        program.emplace_back("SET", symbol->address - symbol->start_address.value());

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

                throw_error("nieprawidłowe użycie tablicy, linia: ", lineno);
            }
    }
}