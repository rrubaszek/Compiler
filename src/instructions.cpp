#include "instructions.hpp"

#include <iostream>

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

int _assign(const std::string& var, Entity* _entity) {
    if (_entity->name == "") {
        program.emplace_back("SET", _entity->value);
        program.emplace_back("STORE", get_variable_address(var));
    }
    else {
        program.emplace_back("STORE", get_variable_address(var));
    }
    return program.size() - 1;
}

int _read(const std::string& var) {
    program.emplace_back("GET", get_variable_address(var));
    return program.size() - 1;
}

int _write(int address) {
    program.emplace_back("PUT", address);
    return program.size() - 1;
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

void _declare(const std::string& name, symbol type, int a, int b) {
    if(!local) {
        if (global_symbol_table.count(name) == 0) {
            int var_register = allocate_register();
            symbol_entry entry = { type, var_register, a, b };  
            global_symbol_table[name] = entry;
            std::cout << "Dodano do pamięci zmienną: " << name << "\n";
        }
        else {
            yyerror("Variable redeclaration");
        }
    } 
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

int _if_stmt(int cond_addr, int commands_addr) {
    program.insert(program.begin() + cond_addr + 1, { "JUMP", commands_addr - cond_addr + 1 });
    return program.size() - 1;
}

int _if_else_stmt(int cond_addr, int commands_addr, int else_addr) {
    program.insert(program.begin() + cond_addr + 1, { "JUMP", commands_addr - cond_addr + 2 });
    program.insert(program.begin() + commands_addr + 2, { "JUMP", else_addr - commands_addr + 1 });
    return program.size() - 1;
}

int _while_stmt(int cond_addr, int commands_addr) {
    program.insert(program.begin() + cond_addr + 1, { "JUMP", commands_addr - cond_addr + 2 });
    program.insert(program.begin() + commands_addr + 2, { "JUMP", cond_addr - commands_addr - 4 });
    return program.size() - 1;
}

int _repeat_stmt(int commands_addr, int cond_addr) {
    int start = program.size();
    std::cout << commands_addr << "\n";
    std::cout << cond_addr << "\n";
    program.insert(program.begin() + cond_addr + 1, { "JUMP", -cond_addr });
    return program.size() - 1;
}

int _for_stmt(const std::string& var, int start, int end, int commands_addr) {

}

int _for_dec_stmt(const std::string& var, int start, int end, int commands_addr) {

}

int _eq(int a, int b) {
    program.emplace_back("LOAD", a);
    program.emplace_back("SUB", b);
    program.emplace_back("JZERO", 2);

    return program.size() - 1; // Return pointer to the program instruction with jump
}

int _neq(int a, int b) {
    program.emplace_back("LOAD", a);
    program.emplace_back("SUB", b);
    program.emplace_back("JZERO", 2); 
    program.emplace_back("JUMP", 2);

    return program.size() - 1; // Return pointer to the program instruction with jump
}

int _gt(int a, int b) {
    program.emplace_back("LOAD", a);
    program.emplace_back("SUB", b);
    program.emplace_back("JPOS", 2); 

    return program.size() - 1; // Return pointer to the program instruction with jump
}

int _le(int a, int b) {
    program.emplace_back("LOAD", a);
    program.emplace_back("SUB", b);
    program.emplace_back("JNEG", 2); 

    return program.size() - 1; // Return pointer to the program instruction with jump
}

int _geq(int a, int b) {
    program.emplace_back("LOAD", a);
    program.emplace_back("SUB", b);
    program.emplace_back("JPOS", 3); 
    program.emplace_back("JZERO", 2);

    return program.size() - 1; // Return pointer to the program instruction with jump
}

int _leq(int a, int b) {
    program.emplace_back("LOAD", a);
    program.emplace_back("SUB", b);
    program.emplace_back("JNEG", 3); 
    program.emplace_back("JZERO", 2);

    return program.size() - 1; // Return pointer to the program instruction with jump
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
    return new Entity(_entity_l->value + _entity_r->value, -1, "temp");
}

Entity* _sub(Entity* _entity_l, Entity* _entity_r) {
    if (_entity_l->address == -1 && _entity_r->address == -1) {
        //program.emplace_back("SET", _entity_l->value + _entity_r->value);
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
    return new Entity(_entity_l->value - _entity_r->value, -1, "temp");
}

Entity* _mul(Entity* _entity_l, Entity* _entity_r) {
    int a = _entity_l->value;;
    int b = _entity_r->value;;
    int a_addr = _entity_l->address;
    int b_addr = _entity_r->address;
    if (_entity_l->address == -1 && _entity_r->address == -1) {
        return new Entity(_entity_l->value * _entity_r->value, -1, "");
    }
    else if (_entity_l->address == -1 && _entity_r->address != -1) {
        program.emplace_back("SET", _entity_l->value);
        a = _entity_l->value;
        a_addr = allocate_register();
        program.emplace_back("STORE", a_addr);
    }
    else if (_entity_r->address == -1 && _entity_l->address != -1) {
        program.emplace_back("SET", _entity_r->value);
        b = _entity_r->value;
        b_addr = allocate_register();
        program.emplace_back("STORE", b_addr);
    }

    if (a > b) {
        std::swap(a, b);
        std::swap(a_addr, b_addr);
    }

    int res_addr = allocate_register();

    program.emplace_back("SET", 0);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JZERO", 10);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("JZERO", 8);

    program.emplace_back("LOAD", res_addr);
    program.emplace_back("ADD", a_addr);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("SET", -1); 
    program.emplace_back("ADD", b_addr);
    program.emplace_back("STORE", b_addr);
    program.emplace_back("JPOS", -6);

    program.emplace_back("LOAD", res_addr);

    free_register(a_addr);

    return new Entity(_entity_l->value * _entity_r->value, -1, "temp");
}

Entity* _div(Entity* _entity_l, Entity* _entity_r) {
    if (_entity_r->address == -1 && _entity_r->value == 2) {
        program.emplace_back("LOAD", _entity_l->address); 
        program.emplace_back("HALF");
        return new Entity(_entity_l->value / 2, -1, "temp");
    }

    int a_addr = _entity_l->address;
    int b_addr = _entity_r->address;

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

    int quotient_reg = allocate_register(); 
    int remainder_reg = allocate_register(); 

    program.emplace_back("SET", 0); 
    program.emplace_back("STORE", quotient_reg);

    program.emplace_back("LOAD", a_addr); 
    program.emplace_back("STORE", remainder_reg);

    program.emplace_back("LOAD", remainder_reg); 

    std::cout << "RIGHT VALUE " << _entity_r->value << "\n";
    if (_entity_r->value > 0) {
        program.emplace_back("SUB", b_addr);
    }
    else {
        program.emplace_back("ADD", b_addr);
    }
    program.emplace_back("JNEG", 8); // Exit loop 

    program.emplace_back("SET", 1);
    program.emplace_back("ADD", quotient_reg);
    program.emplace_back("STORE", quotient_reg);

    program.emplace_back("LOAD", remainder_reg);
    if (_entity_r->value > 0) {
        program.emplace_back("SUB", b_addr);
    }
    else {
        program.emplace_back("ADD", b_addr);
    }
    program.emplace_back("STORE", remainder_reg);

    program.emplace_back("JUMP", -9);

    program.emplace_back("LOAD", quotient_reg); 

    free_register(remainder_reg);
    free_register(quotient_reg);

    return new Entity(_entity_l->value / _entity_r->value, -1, "temp");
}

Entity* _mod(Entity* _entity_l, Entity* _entity_r) {
    int a_addr = _entity_l->address;
    int b_addr = _entity_r->address;

    if (_entity_l->address == -1 && _entity_r->address == -1) {
        return new Entity(_entity_l->value % _entity_r->value, -1, "");
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

    int res_addr = allocate_register();

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JZERO", 10);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("LOAD", res_addr);
    if (_entity_r->value > 0) {
        program.emplace_back("SUB", b_addr);
    }
    else {
        program.emplace_back("ADD", b_addr);
    }
    program.emplace_back("STORE", res_addr);

    program.emplace_back("JNEG", 2);
    program.emplace_back("JUMP", -4);

    program.emplace_back("ADD", b_addr);

    free_register(res_addr);

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

void free_register(int reg) {
    std::cout << "free register " << reg << "\n";
    //free_registers.push(reg);
    if (next_free_register >= global_symbol_table.size())
        next_free_register--;
}