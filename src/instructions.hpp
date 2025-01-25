#pragma once 

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

struct instruction {
	std::string opcode;
	std::optional<int> operand;

	instruction(const std::string& opcode, std::optional<int> operand = std::nullopt) : 
	opcode(opcode),
	operand(operand) 
	{}
};

struct Entity {
	int value;
	int address;
	int start_block_address;
	std::string name;
	std::vector<instruction> program;

	Entity(int value, int address, std::string name) :
	value(value),
	address(address),
	name(name)
	{}
};

struct symbol_entry {   
	int address;
	int zero_address; 
	bool is_iterator;  
};

extern std::vector<instruction> program;
extern std::unordered_map<std::string, symbol_entry> global_symbol_table;
extern std::stack<std::unordered_map<std::string, symbol_entry>> local_symbol_stack;

extern bool is_local;
extern int next_free_register;

symbol_entry* find_symbol(const std::string& name);
void add_symbol(const std::string& name, int zero_index, int size, bool is_local, bool is_iterator);
void remove_symbol(const std::string& name);

int allocate_register();
void free_register(int reg);

Entity* _mul(Entity* a, Entity* b);
Entity* _div(Entity* a, Entity* b);
Entity* _mod(Entity* a, Entity* b);