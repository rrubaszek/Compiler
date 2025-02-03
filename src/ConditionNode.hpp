#pragma once

#include "Node.hpp"
#include "ValueNode.hpp"

class ConditionNode : public Node {
public:
    enum ConditionType { EQ, NEQ, GT, LT, GEQ, LEQ };
    enum ConditionResult { TRUE, FALSE, NONE };
    ConditionType type;
    ConditionResult res = NONE;

    ValueNode* left;
    ValueNode* right;

    ConditionNode() = default;
    ~ConditionNode();
    void compile() override;
    void preprocessing() override {};

private:
    void compile_eq();
    void compile_neq();
    void compile_gt();
    void compile_lt();
    void compile_geq();
    void compile_leq();
};