#pragma once

#include "Node.hpp"

class ValueNode : public Node {
public:
    int value;
    std::string name;

    ValueNode() = default;
    void compile() override;
};