#include "ExpressionNode.hpp"
#include "instructions.hpp"

void ExpressionNode::compile()  {
    switch (type) {
        case VALUE: compile_value(); break;
        case ADD: compile_add(); break;
        case SUB: compile_sub(); break;
        case MUL: compile_mul(); break;
        case DIV: compile_div(); break;
        case MOD: compile_mod(); break;
    }
}

void ExpressionNode::compile_value() {
    left->compile();
    return;
}

void ExpressionNode::compile_add() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        program.emplace_back("SET", left->value + right->value);
        return;
    }

    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::VARIABLE) {
       program.emplace_back("SET", left->value);
       program.emplace_back("ADD", find_symbol(right->name)->address); 
       return;
    }

    if (left->type == ValueNode::ValueType::VARIABLE && right->type == ValueNode::ValueType::CONSTANT) {
       program.emplace_back("SET", right->value);
       program.emplace_back("ADD", find_symbol(left->name)->address); 
       return;
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("ADD", temp);
    free_register(temp);
}

void ExpressionNode::compile_sub() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        program.emplace_back("SET", left->value - right->value);
        return;
    }

    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::VARIABLE) {
       program.emplace_back("SET", left->value);
       program.emplace_back("SUB", find_symbol(right->name)->address); 
       return;
    }

    if (left->type == ValueNode::ValueType::VARIABLE && right->type == ValueNode::ValueType::CONSTANT) {
       program.emplace_back("SET", -(right->value));
       program.emplace_back("ADD", find_symbol(left->name)->address); 
       return;
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    free_register(temp);

}
void ExpressionNode::compile_mul() {}
void ExpressionNode::compile_div() {}
void ExpressionNode::compile_mod() {}