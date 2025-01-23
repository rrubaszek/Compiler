#pragma once 

#include "Node.hpp"

class ExpressionNode : public Node {
public:
    int value;
    enum ExpressionType { ADD, SUB, MUL, DIV };
    ExpressionType type;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;

    ExpressionNode() = default;
    void compile() override;

private:
    void compile_add() {}
    void compile_sub() {}
    void compile_mul() {}
    void compile_div() {}
};