#pragma once 

#include "Node.hpp"
#include "CommandNode.hpp"

class CommandsNode : public Node {
public:
    std::vector<CommandNode*> commands;

    CommandsNode() = default;
    ~CommandsNode();
    void compile() override;
    void preprocessing() override;
};