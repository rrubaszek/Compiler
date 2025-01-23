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
    program.emplace_back("SET", value);
    return;
}

void ExpressionNode::compile_add() {
    if (left->name == "" && right->name == "") {
        program.emplace_back("SET", left->value + right->value);
        return;
    }

    if (left->name == "" && right->name != "") {
       program.emplace_back("SET", left->value);
       program.emplace_back("ADD", find_symbol(right->name)->address); // Here implement find variable address by name from symbol_table
       return;
    }

    if (left->name != "" && right->name == "") {
       program.emplace_back("SET", right->value);
       program.emplace_back("ADD", find_symbol(left->name)->address); // Here implement find variable address by name from symbol_table
       return;
    }

    program.emplace_back("LOAD", find_symbol(left->name)->address);
    program.emplace_back("ADD", find_symbol(right->name)->address);
}

void ExpressionNode::compile_sub() {}
void ExpressionNode::compile_mul() {}
void ExpressionNode::compile_div() {}
void ExpressionNode::compile_mod() {}