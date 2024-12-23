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

struct argument {
    std::string name;  // Argument name
    symbol type;  // Argument type (optional, e.g., "int", "float", "")

	argument(std::string name, symbol type) :
	name(std::move(name)),
	type(type)
	{}
};

struct procedure {
    std::string name;
    int start_address;    // Address of the procedure's first instruction
};


extern std::vector<instruction> program;
extern std::unordered_map<std::string, symbol_entry> global_symbol_table;
extern std::unordered_map<std::string, symbol_entry> local_symbol_table;
extern std::unordered_map<std::string, procedure> procedure_table;
extern std::vector<argument> proc_symbol_table;
extern std::stack<int> free_registers;

int get_variable_address(const std::string& name);
int allocate_register();
void free_register(int reg);

void _put_halt();
int _assign(const std::string& var, int address);
int _read(const std::string& var);
int _write(int address);

void _put_rtrn();
void _procedure_head(const std::string& name);
void _declare(const std::string& name, symbol type, int a, int b);
void _declare_local(const std::string& name, symbol type, int a, int b);
void _declare_arguments(const std::string& name, symbol type);
void _call_procedure();

int _if_stmt(int cond_addr, int commands_addr);
int _if_else_stmt(int cond_addr, int commands_addr, int else_addr);
int _while_stmt(int cond_addr, int commands_addr);
int _repeat_stmt(int commands_addr, int cond_addr);
int _for_stmt(const std::string& var, int start, int end, int commands_addr);
int _for_dec_stmt(const std::string& var, int start, int end, int commands_addr);

int _eq(int a, int b);
int _neq(int a, int b);
int _gt(int a, int b);
int _le(int a, int b);
int _geq(int a, int b);
int _leq(int a, int b);

int _load(int address);
int _add(int l_address, int r_address);
int _sub(int l_address, int r_address);
int _mul(int l_address, int r_address);
int _div(int l_address, int r_address);
int _div2(int address);
int _mod(int l_address, int r_address);
int _set(int value);