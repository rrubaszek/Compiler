#include "ExpressionNode.hpp"

void ExpressionNode::compile()  {
    switch (type) {
        case ADD: compile_add(); break;
        case SUB: compile_sub(); break;
        case MUL: compile_mul(); break;
        case DIV: compile_div(); break;
    }
}