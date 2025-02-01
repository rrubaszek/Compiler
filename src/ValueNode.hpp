#pragma once

#include "Node.hpp"
#include "IdentifierNode.hpp"

class ValueNode : public Node {
public:
    enum ValueType { CONSTANT, OTHER };
    ValueType type;

    bool is_array = false;
    int value;
    int index_value;
    std::string name;
    std::string index_name;

    ValueNode() = default;
    void compile() override;
};