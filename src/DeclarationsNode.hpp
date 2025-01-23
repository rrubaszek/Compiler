#pragma once 

#include "Node.hpp"

class DeclarationsNode : public Node {
public:
    std::vector<std::string> variables;

    DeclarationsNode() = default;
    void compile() override;
};