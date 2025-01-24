#pragma once 

#include "Node.hpp"
#include "ValueNode.hpp"

class ExpressionNode : public Node {
public:
    enum ExpressionType { VALUE, ADD, SUB, MUL, DIV, MOD };
    ExpressionType type;
    
    ValueNode* left;
    ValueNode* right;

    ExpressionNode() = default;
    void compile() override;

private:
    void compile_value();
    void compile_add();
    void compile_sub();
    void compile_mul();
    void compile_div();
    void compile_mod();
};