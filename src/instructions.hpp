#pragma once 

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

    struct instruction {
            std::string opcode;
            std::optional<int> operand;

            instruction(std::string opcode, std::optional<int> operand = std::nullopt) : 
            opcode(std::move(opcode)),
            operand(operand) 
            {}
    };

    extern std::vector<instruction> program;
	extern std::unordered_map<std::string, int> symbol_table;
	extern std::stack<int> free_registers;

	int get_variable_address(const std::string& name);
	int allocate_register();
	void free_register(int reg);

    void _halt();
    void _assign(const std::string& var);
    void _read(const std::string& var);
    void _write(int address);
    void _declare(const std::string& name);

    int _load(int address);
    int _add(int l_address, int r_address);
    int _sub(int l_address, int r_address);
    int _mul(int l_address, int r_address);
    int _div(int l_address, int r_address);
    int _div2(int address);
    int _mod(int l_address, int r_address);
    int _set(int value);