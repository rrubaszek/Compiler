#pragma once

#include "Node.hpp"

class IdentifierNode : public Node {
public:
    std::string name;

    IdentifierNode() = default;
    void compile() override;
};