#pragma once 

#include "Node.hpp"
#include "DeclarationsNode.hpp"
#include "CommandsNode.hpp"

class ProcedureNode : public Node {
public:
    std::string name;
    std::vector<std::string> args;
    DeclarationsNode* declarations;
    CommandsNode* commands;

    ProcedureNode() = default;
    void compile() override;
};