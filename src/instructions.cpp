#include "instructions.hpp"

#include <iostream>

std::unordered_map<std::string, symbol_entry> global_symbol_table;
std::stack<std::unordered_map<std::string, symbol_entry>> local_symbol_stack;
std::vector<instruction> program;
std::unordered_map<std::string, procedure> procedure_table;

int next_temp_free_register = 1; //Those are for temporary calculations and temp variables
int next_free_register = 11; //Those are for variables, arrays, pointers and so on
bool is_local = false;

procedure* find_procedure(const std::string& name) {
    auto it = procedure_table.find(name);
    if (it != procedure_table.end()) {
        return &it->second;
    }

    return nullptr;
}

void add_procedure(const std::string& name, int address, int relative_address, bool is_called, std::vector<std::pair<std::string, bool>>& args) {
    procedure proc = { name, address, relative_address, is_called, args }; 
    procedure_table[name] = proc;
}

symbol_entry* find_symbol(const std::string& name) {
    if (!local_symbol_stack.empty()) {
        auto& local_symbols = local_symbol_stack.top();
        auto it = local_symbols.find(name);
        if (it != local_symbols.end()) {
            return &it->second;
        }
    }

    auto it = global_symbol_table.find(name);
    if (it != global_symbol_table.end()) {
        return &it->second;
    }

    return nullptr; 
}

void add_symbol(const std::string& name, int address, std::optional<int> start_address, bool is_local, bool is_iterator, bool is_uninitialized, Type type, int size) {
    symbol_entry symbol = {address, start_address, is_iterator, is_uninitialized, type, size};
    if (is_local) {
        local_symbol_stack.top()[name] = symbol;
    } else {
        global_symbol_table[name] = symbol;
    }
}

void remove_symbol(const std::string& name) {
    if (is_local) {
        if (local_symbol_stack.empty()) {
            throw std::runtime_error("Local symbol stack is empty. No local scope exists.");
        }

        auto& current_local_table = local_symbol_stack.top();

        auto it = current_local_table.find(name);
        if (it != current_local_table.end()) {
            current_local_table.erase(it); 
        } 
        else {
            throw std::runtime_error("Symbol not found in the current local scope: " + name);
        }
    }
    else {
        auto it = global_symbol_table.find(name);
        if (it != global_symbol_table.end()) {
            global_symbol_table.erase(it); 
        } 
        else {
            throw std::runtime_error("Symbol not found in the global scope: " + name);
        }
    }
}

int allocate_register() {
    return next_free_register++;
}

int allocate_temp_register() {
    if (next_temp_free_register + 1 >= 10) {
        std::cerr << "Cannot allocate more temp registers\n";
        exit(1);
    }
    return next_temp_free_register++;
}

void free_register(int reg) {
    if (next_free_register >= global_symbol_table.size())
        next_free_register--;
}

void free_temp_register(int temp) {
    if (next_temp_free_register > 0)
        next_temp_free_register--;
} 

void throw_error(const std::string& s, int lineno) {
    std::cerr << "ERROR: " << s << lineno << std::endl;
    //exit(1);
}