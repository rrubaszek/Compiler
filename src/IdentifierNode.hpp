#pragma once

#include "Node.hpp"

class IdentifierNode : public Node {
public:
    std::string name;
    std::string index_name;
    ll index_value;
    bool is_array = false;
    bool is_iterator = false;
    bool is_pointer = false; // It can be recognized later, not in grammar

    IdentifierNode() = default;
    ~IdentifierNode() = default;
    void compile() override;
    void preprocessing() override {};
};