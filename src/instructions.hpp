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
	int operand;

	instruction(const std::string& opcode, int operand) : 
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
	int size;   
};

extern std::vector<instruction> program;
extern std::unordered_map<std::string, symbol_entry> global_symbol_table;
extern std::stack<std::unordered_map<std::string, symbol_entry>> local_symbol_stack;

extern bool is_local;
extern int next_free_register;

symbol_entry* find_symbol(const std::string& name);
void add_symbol(const std::string& name, int address, int size, bool is_local);

int get_size();
int get_variable_address(const std::string& name);
int allocate_register();
void free_register(int reg);

Entity* _assign(const std::string& var, Entity* _entity);
Entity* _read(const std::string& var);
Entity* _write(int address);

Entity* _if_stmt(Entity* cond_addr, Entity* commands_addr);
Entity* _if_else_stmt(const std::pair<int, int>* cond_addr, 
					Entity* commands_addr, 
					Entity* else_addr);
std::pair<int, int>* _while_stmt(int cond_addr, int commands_addr);
std::pair<int, int>* _repeat_stmt(int commands_addr, int cond_addr);
std::pair<int, int>* _for_stmt(const std::string& var, Entity* start, Entity* end, std::pair<int, int>* commands_addr);
std::pair<int, int>* _for_dec_stmt(const std::string& var, Entity* start, Entity* end, std::pair<int, int>* commands_addr);

Entity* _eq(Entity* a, Entity* b);
std::pair<int, int>* _neq(Entity* a, Entity* b);
std::pair<int, int>* _gt(Entity* a, Entity* b);
std::pair<int, int>* _le(Entity* a, Entity* b);
std::pair<int, int>* _geq(Entity* a, Entity* b);
std::pair<int, int>* _leq(Entity* a, Entity* b);

Entity* _add(Entity* a, Entity* b);
Entity* _sub(Entity* a, Entity* b);
Entity* _mul(Entity* a, Entity* b);
Entity* _div(Entity* a, Entity* b);
Entity* _mod(Entity* a, Entity* b);