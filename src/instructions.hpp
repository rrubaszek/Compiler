#pragma once 

#include <optional>
#include <string>

struct instruction {
        std::string opcode;
        std::optional<int> operand;

        instruction(std::string opcode, std::optional<int> operand = std::nullopt) : 
            opcode(std::move(opcode)),
            operand(operand) 
            {}
    };