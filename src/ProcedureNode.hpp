#pragma once 

#include "Node.hpp"
#include "DeclarationsNode.hpp"
#include "CommandsNode.hpp"

class ProcedureNode : public Node {
public:
    std::string name;
    std::vector<std::pair<std::string, bool>> args;
    DeclarationsNode* declarations;
    CommandsNode* commands;

    ProcedureNode() = default;
    ~ProcedureNode();
    void compile() override;
    void preprocessing() override;
};