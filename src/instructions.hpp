#pragma once 

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

using ll = long long;

struct instruction {
	std::string opcode;
	std::optional<ll> operand;

	instruction(const std::string& opcode, std::optional<ll> operand = std::nullopt) : 
	opcode(opcode),
	operand(operand) 
	{}
};


enum Type { VARIABLE, ARRAY, POINTER, ARRAY_POINTER };

struct symbol_entry {   
	ll address;
	std::optional<ll> start_address; 
	bool is_iterator;  
	bool is_uninitialized;
	Type type;
	ll size;
};

struct procedure {
	std::string name;
	ll address;
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

extern bool first_pass;
extern bool is_local;
extern int yylineno;
extern ll next_free_register;
extern int next_temp_free_register;

symbol_entry* find_symbol(const std::string& name);
void add_symbol(const std::string& name, ll address, std::optional<ll> start_address, bool is_local, bool is_iterator, bool is_uninitialized, Type type, ll size);
void remove_symbol(const std::string& name);

procedure* find_procedure(const std::string& name);
void add_procedure(const std::string& name, ll address, int relative_address, bool is_called, std::vector<std::pair<std::string, bool>>& args);

ll allocate_register();
void free_register(ll reg);

int allocate_temp_register();
void free_temp_register(int temp);

void throw_error(const std::string& s, int lineno);

void mark_called_procs(const std::string& name);
void remove_unused_procs();