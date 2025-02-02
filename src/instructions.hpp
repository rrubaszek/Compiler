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

enum Type { VARIABLE, ARRAY, POINTER, ARRAY_POINTER };

struct symbol_entry {   
	int address;
	std::optional<int> start_address; 
	bool is_iterator;  
	bool is_uninitialized;
	Type type;
	int size;
};

struct procedure {
	std::string name;
	int address;
	int relative_address; // Line no in program to calculate jumps for proc_call
	bool is_called;
	std::vector<std::string> procs_called_by;
	std::vector<std::pair<std::string, bool>> args;
};

extern std::stack<std::string> called_procedures;

extern std::vector<instruction> program;
extern std::unordered_map<std::string, symbol_entry> global_symbol_table;
extern std::stack<std::unordered_map<std::string, symbol_entry>> local_symbol_stack;
extern std::unordered_map<std::string, procedure> procedure_table;

extern bool is_local;
extern int yylineno;
extern int next_free_register;
extern int next_temp_free_register;

symbol_entry* find_symbol(const std::string& name);
void add_symbol(const std::string& name, int address, std::optional<int> start_address, bool is_local, bool is_iterator, bool is_uninitialized, Type type, int size);
void remove_symbol(const std::string& name);

procedure* find_procedure(const std::string& name);
void add_procedure(const std::string& name, int address, int relative_address, bool is_called, std::vector<std::pair<std::string, bool>>& args);

int allocate_register();
void free_register(int reg);

int allocate_temp_register();
void free_temp_register(int temp);

void throw_error(const std::string& s, int lineno);

void mark_called_procs(const std::string& name);
void remove_unused_procs();