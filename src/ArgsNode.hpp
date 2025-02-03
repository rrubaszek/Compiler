#pragma once 

#include "Node.hpp"

class ArgsNode : public Node {
public:
    ArgsNode() = default;
    ~ArgsNode() = default;
    void compile() override {};
    void preprocessing() override {};

    std::vector<std::string> args;
};