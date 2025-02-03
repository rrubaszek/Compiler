#pragma once 

#include "Node.hpp"
#include "ProcedureNode.hpp"
#include "MainNode.hpp"

class ProgramNode : public Node {
public:
    std::vector<ProcedureNode*> procedures;
    MainNode* main;

    ProgramNode() = default;
    ~ProgramNode();
    void compile() override;
    void preprocessing() override;
};