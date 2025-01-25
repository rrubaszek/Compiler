#pragma once

#include "Node.hpp"

class IdentifierNode : public Node {
public:
    std::string name;
    std::string index_name;
    int index_value;
    bool is_array = false;
    bool is_iterator = false;

    IdentifierNode() = default;
    void compile() override;
};