#include "instructions.hpp"

#include <iostream>

// For conditions
#define FALSE 0
#define TRUE 1
#define NONE 2

std::unordered_map<std::string, symbol_entry> global_symbol_table;
std::vector<instruction> program;

static int next_free_register = 1;
bool local = false;

void yyerror(const char *s);

void _put_halt() {
    program.emplace_back("HALT");
}

void _put_rtrn() {
    program.emplace_back("RTRN", 1);
}

void _append_to_main_program(Entity* e) {
    for (instruction i : e->program) {
        program.emplace_back(i.opcode, i.operand);
    }
}

Entity* _assign(const std::string& var, Entity* e) {
    Entity* entity = new Entity(-1, -1, "");
    if (e->name == "") {
        entity->program.emplace_back("SET", e->value);
        entity->program.emplace_back("STORE", get_variable_address(var));
    }
    else {
        entity->program.emplace_back("STORE", get_variable_address(var));
    }
    return entity;
}

Entity* _read(const std::string& var) {
    Entity* entity = new Entity(-1, -1, "");
    entity->program.emplace_back("GET", get_variable_address(var));
    return entity;
}

Entity* _write(int address) {
    Entity* entity = new Entity(-1, -1, "");
    entity->program.emplace_back("PUT", address);
    return entity;
}

void _procedure_head(const std::string& name) {
    // Procedure proc;
    // proc.name = name;
    // proc.start_address = program.size(); // Address placeholder
    // procedure_table[name] = proc;
    // std::cout << procedure_table[name].name << "\n";

    // for (int i = 0; i < proc_symbol_table.size(); ++i) {
    //     global_symbol_table[proc_symbol_table[i].name] = {proc_symbol_table[i].type, 0, 0, 0}; // Example: map name to location
    // }

    // proc_symbol_table.clear();
}

int _declare(const std::string& name, symbol type, int a, int b) {
    if(!local) {
        if (global_symbol_table.count(name) == 0) {
            int var_register = allocate_register();
            symbol_entry entry = { type, var_register, 0, a, b };  // basic value is 0
            global_symbol_table[name] = entry;
            std::cout << "Dodano do pamięci zmienną: " << name << "\n";
            return var_register;
        }
        else {
            yyerror("Variable redeclaration");
        }
    } 

    return 0;
    // else {
    //     if (local_symbol_table.count(name) == 0) {
    //         int var_register = allocate_register();
    //         symbol_entry entry = { type, var_register, a, b };  
    //         local_symbol_table[name] = entry;
    //         std::cout << "Dodano do pamięci zmienną lokalna: " << name << "\n";
    //     }
    //     else {
    //         yyerror("Variable redeclaration");
    //     }
    // }
}

void _declare_arguments(const std::string& name, symbol type) {
    //proc_symbol_table.emplace_back(name, type);
}

void _call_procedure(const std::string& name) {
    // if (procedure_table.find(name) == procedure_table.end()) {
    //     yyerror("Undefined procedure\n");
    // }

    // // Generate JUMP to procedure
    // int proc_address = procedure_table[name].start_address;
    // program.emplace_back("CALL", proc_address); // Jump to procedure
}

Entity* _if_stmt(const std::pair<int, int>* cond_addr, Entity* commands_addr) {
    Entity* entity = new Entity(-1, -1, "");
    if (cond_addr->second == TRUE) {
        for (instruction i : commands_addr->program) {
            entity->program.emplace_back(i.opcode, i.operand);
        }
        return entity;
    }

    if (cond_addr->second == FALSE) {
        return entity;
    }

    entity->program.emplace_back("JUMP", )
    //program.insert(program.begin() + cond_addr->first + 1, { "JUMP", commands_addr->second - cond_addr->first + 1 });
    // for (instruction i : commands_addr->program) {
    //         entity->program.emplace_back(i.opcode, i.operand);
    // }
    return entity;
}

// Entity* _if_else_stmt(const std::pair<int, int>* cond_addr, 
//                                 const std::pair<int, int>* commands_addr, 
//                                 const std::pair<int, int>* else_addr) {
//     Entity* entity = new Entity(-1, -1, "");
//     if (cond_addr->second == TRUE) {
//         program.erase(program.begin() + commands_addr->second + 1, program.begin() + else_addr->second + 1);
//         return entity;
//     }

//     if (cond_addr->second == FALSE) {
//         program.erase(program.begin() + cond_addr->first + 1, program.begin() + commands_addr->second + 1);
//         return entity;
//     }

//     //program.insert(program.begin() + cond_addr->first + 1, { "JUMP", commands_addr->second - cond_addr->first + 2 });
//     //program.insert(program.begin() + commands_addr->second + 2, { "JUMP", else_addr->second - commands_addr->second + 1 });
//     entity->program.emplace_back("JUMP", commands_addr->second - cond_addr->first + 2);
//     entity->program.emplace_back("JUMP", else_addr->second - commands_addr->second + 1);
//     return entity;
// }

std::pair<int, int> *_while_stmt(int cond_addr, int commands_addr) {
    // TODO: finish
    int start = program.size() - 1;
    program.insert(program.begin() + cond_addr + 1, { "JUMP", commands_addr - cond_addr + 2 });
    program.insert(program.begin() + commands_addr + 2, { "JUMP", cond_addr - commands_addr - 4 });
    return new std::pair<int, int>(start, program.size() - 1);
}

std::pair<int, int> *_repeat_stmt(int commands_addr, int cond_addr) {
    // TODO: finish
    int start = program.size();
    program.insert(program.begin() + cond_addr + 1, { "JUMP", -cond_addr });
    return new std::pair<int, int>(start, program.size() - 1);
}

std::pair<int, int> *_for_stmt(const std::string& var, Entity* start, Entity* end, std::pair<int, int>* commands_addr) {
    int size = program.size() - 1;
    int i_addr = _declare(var, SCALAR, 0, 0);

    if (start->address == -1) { 
        program.insert(program.begin() + commands_addr->first - 1, { "SET", start->value });
        program.insert(program.begin() + commands_addr->first, { "STORE", i_addr });
    } 
    else {
        program.insert(program.begin() + commands_addr->first - 1, { "LOAD", start->address });
        program.insert(program.begin() + commands_addr->first, { "STORE", i_addr });
    }
    
    if (end->address == -1) {       
        program.insert(program.begin() + commands_addr->first + 1, { "SET", end->value });  
        program.insert(program.begin() + commands_addr->first + 2, { "SUB", i_addr });
    } else {    
        program.insert(program.begin() + commands_addr->first + 1, { "LOAD", end->address });                               
        program.insert(program.begin() + commands_addr->first + 2, { "SUB", i_addr });
    }

    program.insert(program.begin() + commands_addr->first + 3, { "JNEG", commands_addr->second});

    std::cout << commands_addr->first << " " << commands_addr->second << "\n";

    program.insert(program.begin() + commands_addr->second + 1, { "SET", 1 });
    program.insert(program.begin() + commands_addr->second + 2, { "ADD", i_addr });
    program.insert(program.begin() + commands_addr->second + 3, { "STORE", i_addr });

    program.emplace_back("JUMP", -10);

    return new std::pair<int, int>(size, program.size() - 1);
}

std::pair<int, int> * _for_dec_stmt(const std::string& var, Entity* start, Entity* end, std::pair<int, int>* commands_addr) {
    // TODO: finish
}

std::pair<int, int> *_eq(Entity* a, Entity* b) {
    if (a->address == -1 && b->address == -1) {
        if (a->value == b->value) {
            return new std::pair<int, int>(program.size() - 1, TRUE); // always true, dont put the condition
        }
        else {
            return new std::pair<int, int>(program.size() - 1, FALSE); // always false, remove code block after condition
        }
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JZERO", 2);
    }
    else if (a->address != -1 && b->address == -1) {
        program.emplace_back("SET", b->value);
        program.emplace_back("SUB", a->address);
        program.emplace_back("JZERO", 2);
    }
    else {
        program.emplace_back("LOAD", a->address);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JZERO", 2);
    }

    return new std::pair<int, int>(program.size() - 1, NONE); // Return pointer to the program instruction with jump
}

std::pair<int, int> * _neq(Entity* a, Entity* b) {
    if (a->address == -1 && b->address == -1) {
        if (a->value != b->value) {
            return new std::pair<int, int>(program.size() - 1, TRUE); // always true, dont put the condition
        }
        else {
            return new std::pair<int, int>(program.size() - 1, FALSE); // always false, remove code block after condition
        }
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JZERO", 2); 
        program.emplace_back("JUMP", 2);
    }
    else if (a->address != -1 && b->address == -1) {
        program.emplace_back("SET", b->value);
        program.emplace_back("SUB", a->address);
        program.emplace_back("JZERO", 2); 
        program.emplace_back("JUMP", 2);
    }
    else {
        program.emplace_back("LOAD", a->address);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JZERO", 2);
        program.emplace_back("JUMP", 2);
    }

    return new std::pair<int, int>(program.size() - 1, NONE); 
}

std::pair<int, int> * _gt(Entity* a, Entity* b) {
    if (a->address == -1 && b->address == -1) {
        if (a->value > b->value) {
            return new std::pair<int, int>(program.size() - 1, TRUE); // always true, dont put the condition
        }
        else {
            return new std::pair<int, int>(program.size() - 1, FALSE); // always false, remove code block after condition
        }
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JPOS", 2); 
    }
    else if (a->address != -1 && b->address == -1) {
        program.emplace_back("SET", b->value);
        program.emplace_back("SUB", a->address);
        program.emplace_back("JPOS", 2); 
    }
    else {
        program.emplace_back("LOAD", a->address);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JPOS", 2);
    } 

    return new std::pair<int, int>(program.size() - 1, NONE); 
}

std::pair<int, int> * _le(Entity* a, Entity* b) {
    if (a->address == -1 && b->address == -1) {
        if (a->value < b->value) {
            return new std::pair<int, int>(program.size() - 1, TRUE); // always true, dont put the condition
        }
        else {
            return new std::pair<int, int>(program.size() - 1, FALSE); // always false, remove code block after condition
        }
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JNEG", 2); 
    }
    else if (a->address != -1 && b->address == -1) {
        program.emplace_back("SET", b->value);
        program.emplace_back("SUB", a->address);
        program.emplace_back("JNEG", 2); 
    }
    else {
        program.emplace_back("LOAD", a->address);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JNEG", 2);
    }

    return new std::pair<int, int>(program.size() - 1, NONE); 
}

std::pair<int, int> * _geq(Entity* a, Entity* b) {
    if (a->address == -1 && b->address == -1) {
        if (a->value >= b->value) {
            return new std::pair<int, int>(program.size() - 1, TRUE); // always true, dont put the condition
        }
        else {
            return new std::pair<int, int>(program.size() - 1, FALSE); // always false, remove code block after condition
        }
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JPOS", 3); 
        program.emplace_back("JZERO", 2);
    }
    else if (a->address != -1 && b->address == -1) {
        program.emplace_back("SET", b->value);
        program.emplace_back("SUB", a->address);
        program.emplace_back("JPOS", 3); 
        program.emplace_back("JZERO", 2);
    }
    else {
        program.emplace_back("LOAD", a->address);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JPOS", 3);
        program.emplace_back("JZERO", 2);
    }

    return new std::pair<int, int>(program.size() - 1, NONE); 
}

std::pair<int, int> * _leq(Entity* a, Entity* b) {
    if (a->address == -1 && b->address == -1) {
        if (a->value <= b->value) {
            return new std::pair<int, int>(program.size() - 1, TRUE); // always true, dont put the condition
        }
        else {
            return new std::pair<int, int>(program.size() - 1, FALSE); // always false, remove code block after condition
        }
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JNEG", 3); 
        program.emplace_back("JZERO", 2);
    }
    else if (a->address != -1 && b->address == -1) {
        program.emplace_back("SET", b->value);
        program.emplace_back("SUB", a->address);
        program.emplace_back("JNEG", 3); 
        program.emplace_back("JZERO", 2);
    }
    else {
        program.emplace_back("LOAD", a->address);
        program.emplace_back("SUB", b->address);
        program.emplace_back("JNEG", 3);
        program.emplace_back("JZERO", 2);
    }

    return new std::pair<int, int>(program.size() - 1, NONE); 
}

int _load(Entity _entity) {
    program.emplace_back("LOAD", _entity.address);
    return _entity.address;
}

Entity* _add(Entity* _entity_l, Entity* _entity_r) {
    if (_entity_l->address == -1 && _entity_r->address == -1) {
        //program.emplace_back("SET", _entity_l->value + _entity_r->value);
        return new Entity(_entity_l->value + _entity_r->value, -1, "");
    }
    else if (_entity_l->address == -1 && _entity_r->address != -1) {
        program.emplace_back("SET", _entity_l->value);
        program.emplace_back("ADD", _entity_r->address);
    }
    else if (_entity_r->address == -1 && _entity_l->address != -1) {
        program.emplace_back("SET", _entity_r->value);
        program.emplace_back("ADD", _entity_l->address);
    }
    else {
        program.emplace_back("LOAD", _entity_l->address);
        program.emplace_back("ADD", _entity_r->address);
    }
    return new Entity(-1, -1, "temp");
}

Entity* _sub(Entity* _entity_l, Entity* _entity_r) {
    if (_entity_l->address == -1 && _entity_r->address == -1) {
        return new Entity(_entity_l->value - _entity_r->value, -1, "");
    }
    else if (_entity_l->address == -1 && _entity_r->address != -1) {
        program.emplace_back("SET", _entity_l->value);
        program.emplace_back("SUB", _entity_r->address);
    }
    else if (_entity_r->address == -1 && _entity_l->address != -1) {
        program.emplace_back("SET", _entity_r->value);
        program.emplace_back("SUB", _entity_l->address);
    }
    else {
        program.emplace_back("LOAD", _entity_l->address);
        program.emplace_back("SUB", _entity_r->address);
    }
    return new Entity(-1, -1, "temp");
}

Entity* _mul(Entity* _entity_l, Entity* _entity_r) {
    int a_addr = _entity_l->address;
    int b_addr = _entity_r->address;

    if (_entity_l->address == -1 && _entity_r->address == -1) {
        return new Entity(_entity_l->value * _entity_r->value, -1, "");
    }

    // Entities has values only when they are constants (address is -1)
    if (_entity_l->address == -1 && _entity_r->address != -1) {
        program.emplace_back("SET", _entity_l->value);
        a_addr = allocate_register();
        program.emplace_back("STORE", a_addr);
    } else if (_entity_r->address == -1 && _entity_l->address != -1) {
        program.emplace_back("SET", _entity_r->value);
        b_addr = allocate_register();
        program.emplace_back("STORE", b_addr);
    }

    int result_addr = allocate_register(); 
      
    program.emplace_back("SET", 0);        
    program.emplace_back("STORE", result_addr);

    // Check if a or b are negative
    int sign_addr = allocate_register();

    program.emplace_back("SET", 1);
    program.emplace_back("STORE", sign_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JPOS", 6);       
    program.emplace_back("SET", -1);      
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);       
    program.emplace_back("SUB", b_addr);
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("JPOS", 7);       
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", sign_addr); 
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);      
    program.emplace_back("SUB", a_addr);
    program.emplace_back("STORE", a_addr);

    // Main multiplication loop
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JZERO", 18);     // Exit if b == 0

    int check_parity = allocate_register();

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", check_parity); 
    program.emplace_back("LOAD", check_parity);
    program.emplace_back("ADD", check_parity);
    program.emplace_back("SUB", b_addr);
    program.emplace_back("JZERO", 4); // If b is even don't add a to the result

    free_register(check_parity);

    // Add a to the result
    program.emplace_back("LOAD", result_addr);
    program.emplace_back("ADD", a_addr); 
    program.emplace_back("STORE", result_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("ADD", a_addr);
    program.emplace_back("STORE", a_addr);

    program.emplace_back("JUMP", -18); // Back to the main loop

    // Make result with right sign
    program.emplace_back("LOAD", sign_addr);
    program.emplace_back("JPOS", 4);       
    program.emplace_back("SET", 0);        
    program.emplace_back("SUB", result_addr);
    program.emplace_back("STORE", result_addr);
    program.emplace_back("LOAD", result_addr);

    free_register(sign_addr);
    free_register(result_addr);

    return new Entity(-1, -1, "temp");
}

Entity* _div(Entity* _entity_l, Entity* _entity_r) {
    if (_entity_r->address == -1 && _entity_r->value == 2) {
        program.emplace_back("LOAD", _entity_l->address); 
        program.emplace_back("HALF");
        return new Entity(-1, -1, "temp");
    }

    int a_addr = _entity_l->address;
    int b_addr = _entity_r->address;

    // Entities has value only for constants
    if (_entity_l->address == -1 && _entity_r->address == -1) {
        return new Entity(_entity_l->value / _entity_r->value, -1, "");
    } 
    else if (_entity_l->address == -1 && _entity_r->address != -1) {
        program.emplace_back("SET", _entity_l->value);
        a_addr = allocate_register();
        program.emplace_back("STORE", a_addr);
    } 
    else if (_entity_r->address == -1 && _entity_l->address != -1) {
        program.emplace_back("SET", _entity_r->value);
        b_addr = allocate_register();
        program.emplace_back("STORE", b_addr);
    }

    int quotient_addr = allocate_register(); 
    int remainder_addr = allocate_register(); 
    int temp_addr = allocate_register();      
    int sign_addr = allocate_register();  
    int divisor_sign = allocate_register();   

    program.emplace_back("SET", 1);         
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("STORE", divisor_sign);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JPOS", 7);       
    program.emplace_back("SET", -1);      
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("STORE", divisor_sign);
    program.emplace_back("SET", 0);       
    program.emplace_back("SUB", b_addr);
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("JPOS", 7);       
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", sign_addr); 
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);      
    program.emplace_back("SUB", a_addr);
    program.emplace_back("STORE", a_addr);

    program.emplace_back("SET", 0);    
    program.emplace_back("STORE", quotient_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("STORE", remainder_addr);

    // Find largest b multiply
    program.emplace_back("SET", 1);
    program.emplace_back("STORE", temp_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("ADD", b_addr);
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("ADD", temp_addr);
    program.emplace_back("STORE", temp_addr);
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("SUB", a_addr);
    program.emplace_back("JPOS", 2);
    program.emplace_back("JUMP", -9);

    // Redo b and temp to the previous values
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", temp_addr);

    // Main loop
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("JZERO", 15);

    program.emplace_back("LOAD", remainder_addr);
    program.emplace_back("SUB", b_addr);
    program.emplace_back("JNEG", 5);
    program.emplace_back("STORE", remainder_addr);

    program.emplace_back("LOAD", quotient_addr);
    program.emplace_back("ADD", temp_addr);
    program.emplace_back("STORE", quotient_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", temp_addr);
    program.emplace_back("JUMP", -15);

    program.emplace_back("LOAD", divisor_sign);
    program.emplace_back("JPOS", 4);
    program.emplace_back("SET", 1);
    program.emplace_back("ADD", quotient_addr);
    program.emplace_back("STORE", quotient_addr);

    // Set the right sign
    program.emplace_back("LOAD", sign_addr);
    program.emplace_back("JPOS", 4);
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", quotient_addr);
    program.emplace_back("STORE", quotient_addr);

    program.emplace_back("LOAD", quotient_addr);

    free_register(temp_addr);
    free_register(sign_addr);
    free_register(quotient_addr);
    free_register(remainder_addr);
    free_register(divisor_sign);

    return new Entity(-1, -1, "temp");
}

Entity* _mod(Entity* _entity_l, Entity* _entity_r) {
    int a_addr = _entity_l->address;
    int b_addr = _entity_r->address;

    if (_entity_l->address == -1 && _entity_r->address == -1) {
        return new Entity(_entity_l->value % _entity_r->value, -1, "");
    }

    if (_entity_l->address == -1 && _entity_r->address != -1) {
        program.emplace_back("SET", _entity_l->value);
        a_addr = allocate_register();
        program.emplace_back("STORE", a_addr);
    } else if (_entity_r->address == -1 && _entity_l->address != -1) {
        program.emplace_back("SET", _entity_r->value);
        b_addr = allocate_register();
        program.emplace_back("STORE", b_addr);
    }

    // TODO: Implement rest of the modulo logic

    return new Entity(-1, -1, "temp");
}

int get_variable_address(const std::string& name) {
    // if (local && local_symbol_table.find(name) != local_symbol_table.end()) {
    //     return local_symbol_table[name].address;
    // } 
    // else 
    if (global_symbol_table.find(name) != global_symbol_table.end()) {
        return global_symbol_table[name].address;
    } 
    else {
        yyerror(("Undefined variable: " + name).c_str());
        exit(1);
    }
}

int get_variable_value(const std::string& name) {
    if (global_symbol_table.find(name) != global_symbol_table.end()) {
        return global_symbol_table[name].value;
    } 
    else {
        yyerror(("Undefined variable: " + name).c_str());
        exit(1);
    }
}

int allocate_register() {
    // if (!free_registers.empty()) {
    //     int reg = free_registers.top();
    //     free_registers.pop();
    //     std::cout << "register allocated " << reg << "\n";
    //     return reg;
    // }
    std::cout << "allocated: " << next_free_register << "\n";
    return next_free_register++;
}

int get_size() {
    return program.size();
}

void free_register(int reg) {
    std::cout << "free register " << reg << "\n";
    //free_registers.push(reg);
    if (next_free_register >= global_symbol_table.size())
        next_free_register--;
}