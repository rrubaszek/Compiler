#include "instructions.hpp"

#include <iostream>

std::unordered_map<std::string, symbol_entry> global_symbol_table;
std::stack<std::unordered_map<std::string, symbol_entry>> local_symbol_stack;
std::vector<instruction> program;

int next_free_register = 1;
bool is_local = false;

void yyerror(const char *s);

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

void add_symbol(const std::string& name, int address, int zero_index, bool is_local, bool is_iterator) {
    symbol_entry symbol = {address, zero_index, is_iterator};
    if (is_local) {
        local_symbol_stack.top()[name] = symbol;
        std::cout << "local" << name << " " << address << "\n";
    } else {
        global_symbol_table[name] = symbol;
        std::cout << "global" << name << " " << address << "\n";
    }
}

void remove_symbol(const std::string& name) {
    if (local_symbol_stack.empty()) {
        throw std::runtime_error("Local symbol stack is empty. No local scope exists.");
    }

    auto& current_local_table = local_symbol_stack.top();

    auto it = current_local_table.find(name);
    if (it != current_local_table.end()) {
        current_local_table.erase(it); 
    } else {
        throw std::runtime_error("Symbol not found in the current local scope: " + name);
    }
}

Entity* _mod(Entity* a, Entity* b) {
    Entity* e = new Entity(-1, -1, "temp");
    e->start_block_address = program.size(); 

    int a_addr = a->address;
    int b_addr = b->address;

    if (a->address == -1 && b->address == -1) {
        e->value = a->value % b->value;
        return e;
    }

    if (a->address == -1 && b->address != -1) {
        program.emplace_back("SET", a->value);
        a_addr = allocate_register();
        program.emplace_back("STORE", a_addr);
    } else if (b->address == -1 && a->address != -1) {
        program.emplace_back("SET", b->value);
        b_addr = allocate_register();
        program.emplace_back("STORE", b_addr);
    }

    // TODO: Implement rest of the modulo logic

    return e;
}

int allocate_register() {
    std::cout << "allocated: " << next_free_register << "\n";
    return next_free_register++;
}

void free_register(int reg) {
    std::cout << "free register " << reg << "\n";
    if (next_free_register >= global_symbol_table.size())
        next_free_register--;
}