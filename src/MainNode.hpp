#pragma once 

#include "Node.hpp"
#include "DeclarationsNode.hpp"
#include "CommandsNode.hpp"

struct MainNode : public Node {
public:
    DeclarationsNode* declarations;
    CommandsNode* commands;

    MainNode() = default;
    ~MainNode();
    void compile() override;
    void preprocessing() override;
};