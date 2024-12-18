#pragma once 

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

enum symbol {
    SCALAR,
    ARRAY
};

struct symbol_entry {    
    symbol type; 	
	int address;
	int a;
	int b;    
};

struct instruction {
	std::string opcode;
	std::optional<int> operand;

	instruction(std::string opcode, std::optional<int> operand = std::nullopt) : 
	opcode(std::move(opcode)),
	operand(operand) 
	{}
};

extern std::vector<instruction> program;
extern std::unordered_map<std::string, symbol_entry> symbol_table;
extern std::stack<int> free_registers;

int get_variable_address(const std::string& name);
int allocate_register();
void free_register(int reg);

void _halt();
int _assign(const std::string& var, int address);
int _read(const std::string& var);
int _write(int address);
void _declare(const std::string& name, symbol type, int a, int b);

int _cond_else(int cond_addr, int commands_addr, int else_addr);

int _eq(int a, int b);

int _load(int address);
int _add(int l_address, int r_address);
int _sub(int l_address, int r_address);
int _mul(int l_address, int r_address);
int _div(int l_address, int r_address);
int _div2(int address);
int _mod(int l_address, int r_address);
int _set(int value);