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

struct Entity {
	int value;
	int address;
	std::string name;

	Entity(int value, int address, std::string name) :
	value(value),
	address(address),
	name(name)
	{}
};

struct symbol_entry {    
    symbol type; 	
	int address;
	int value;
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

struct Procedure {
    std::string name;
    int start_address;    // Address of the procedure's first instruction
};

extern std::vector<instruction> program;
extern std::unordered_map<std::string, symbol_entry> global_symbol_table;

int get_size();
int get_variable_address(const std::string& name);
int get_variable_value(const std::string& name);
int allocate_register();
void free_register(int reg);

void _put_halt();
std::pair<int, int>* _assign(const std::string& var, Entity* _entity);
std::pair<int, int>* _read(const std::string& var);
std::pair<int, int>* _write(int address);

void _put_rtrn();
void _procedure_head(const std::string& name);
int _declare(const std::string& name, symbol type, int a, int b);
void _declare_local(const std::string& name, symbol type, int a, int b);
void _declare_arguments(const std::string& name, symbol type);
void _call_procedure();

std::pair<int, int>* _if_stmt(const std::pair<int, int>* cond_addr, 
                            const std::pair<int, int>* commands_addr);
std::pair<int, int>* _if_else_stmt(const std::pair<int, int>* cond_addr, 
								const std::pair<int, int>* commands_addr, 
								const std::pair<int, int>* else_addr);
std::pair<int, int>* _while_stmt(int cond_addr, int commands_addr);
std::pair<int, int>* _repeat_stmt(int commands_addr, int cond_addr);
std::pair<int, int>* _for_stmt(const std::string& var, Entity* start, Entity* end, std::pair<int, int>* commands_addr);
std::pair<int, int>* _for_dec_stmt(const std::string& var, Entity* start, Entity* end, std::pair<int, int>* commands_addr);

std::pair<int, int>* _eq(Entity* a, Entity* b);
std::pair<int, int>* _neq(Entity* a, Entity* b);
std::pair<int, int>* _gt(Entity* a, Entity* b);
std::pair<int, int>* _le(Entity* a, Entity* b);
std::pair<int, int>* _geq(Entity* a, Entity* b);
std::pair<int, int>* _leq(Entity* a, Entity* b);

Entity* _add(Entity* _entity_l, Entity* _entity_r);
Entity* _sub(Entity* _entity_l, Entity* _entity_r);
Entity* _mul(Entity* _entity_l, Entity* _entity_r);
Entity* _div(Entity* _entity_l, Entity* _entity_r);
Entity* _mod(Entity* _entity_l, Entity* _entity_r);