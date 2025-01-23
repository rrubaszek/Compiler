#pragma once 

#include "Node.hpp"

class ArgsNode : public Node {
public:
    ArgsNode() = default;
    std::vector<std::string> args;

    void compile() override;
};