#include "instructions.hpp"

#include <iostream>

std::unordered_map<std::string, symbol_entry> symbol_table;
std::stack<int> free_registers;
std::vector<instruction> program;

static int next_free_register = 1;

void yyerror(const char *s);

void _halt() {
    program.emplace_back("HALT");
}

int _assign(const std::string& var, int address) {
    program.emplace_back("STORE", get_variable_address(var));
    free_register(address);
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

void _declare(const std::string& name, symbol type, int a, int b) {
    if (symbol_table.count(name) == 0) {
        int var_register = allocate_register();
        symbol_entry entry = { type, var_register, a, b };  
        symbol_table[name] = entry;
        std::cout << "Dodano do pamięci zmienną: " << name << "\n";
    }
    else {
        yyerror("Variable redeclaration");
    }
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

int _repeat_stmt(int cond_addr, int commands_addr) {

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

int _load(int address) {
    program.emplace_back("LOAD", address);
    return address;
}

int _add(int l_address, int r_address) {
    program.emplace_back("LOAD", l_address);
    program.emplace_back("ADD", r_address);
    return l_address;
}

int _sub(int l_address, int r_address) {
    program.emplace_back("LOAD", l_address);
    program.emplace_back("SUB", r_address);
    return l_address;
}

int _mul(int l_address, int r_address) {
    int res_addr = allocate_register();

    program.emplace_back("SET", 0);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("LOAD", res_addr);
    program.emplace_back("ADD", l_address);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("SET", -1); 
    program.emplace_back("ADD", r_address);
    program.emplace_back("STORE", r_address);
    program.emplace_back("JPOS", -6);

    program.emplace_back("LOAD", res_addr);

    return res_addr;
}

int _div(int numerator, int denominator) {
    int quotient_reg = allocate_register(); 
    int remainder_reg = allocate_register(); 

    program.emplace_back("SET", 0); 
    program.emplace_back("STORE", quotient_reg);

    program.emplace_back("LOAD", numerator); 
    program.emplace_back("STORE", remainder_reg);

    program.emplace_back("LOAD", remainder_reg); 
    program.emplace_back("SUB", denominator);
    program.emplace_back("JNEG", 8); // Exit loop 

    program.emplace_back("SET", 1);
    program.emplace_back("ADD", quotient_reg);
    program.emplace_back("STORE", quotient_reg);

    program.emplace_back("LOAD", remainder_reg);
    program.emplace_back("SUB", denominator);
    program.emplace_back("STORE", remainder_reg);

    program.emplace_back("JUMP", -9);

    program.emplace_back("LOAD", quotient_reg); 

    free_register(remainder_reg);

    return quotient_reg;
}

int _div2(int address) {
    program.emplace_back("LOAD", address); 
    program.emplace_back("HALF");
    return address;
}

int _mod(int l_address, int r_address) {
    int res_addr = allocate_register();

    program.emplace_back("LOAD", r_address);
    program.emplace_back("JZERO", 10);

    program.emplace_back("LOAD", l_address);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("LOAD", res_addr);
    program.emplace_back("SUB", r_address);
    program.emplace_back("STORE", res_addr);

    program.emplace_back("JNEG", 2);
    program.emplace_back("JUMP", -4);

    program.emplace_back("ADD", r_address);

    return res_addr;
}

int _set(int value) {
    int address = allocate_register();
    program.emplace_back("SET", value);
    program.emplace_back("STORE", address);
    return address;
}

int get_variable_address(const std::string& name) {
    if (symbol_table.count(name) == 0) {
        yyerror(("Undefined variable: " + name).c_str());
        exit(1);
    }
    return symbol_table[name].address;
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
    if (next_free_register > 0)
        next_free_register--;
}