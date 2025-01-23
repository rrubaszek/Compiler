#include "ConditionNode.hpp"
#include "instructions.hpp"

void ConditionNode::compile()  {
    switch (type) {
        case EQ: compile_eq(); break;
        case NEQ: compile_neq(); break;
        case GT: compile_gt(); break;
        case LT: compile_lt(); break;
        case GEQ: compile_geq(); break;
        case LEQ: compile_leq(); break;
    }
}

void ConditionNode::compile_eq() {
    if (left->name == "" && right->name == "") {
        if (left->value == right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->name == "" && right->name != "") {
        program.emplace_back("SET", left->value);
        program.emplace_back("SUB", find_symbol(right->name)->address);
        program.emplace_back("JZERO", 2);
        return;
    }
    
    if (left->name != "" && right->name == "") {
        program.emplace_back("SET", right->value);
        program.emplace_back("SUB", find_symbol(left->name)->address);
        program.emplace_back("JZERO", 2);
        return;
    }
    
    program.emplace_back("LOAD", find_symbol(left->name)->address);
    program.emplace_back("SUB", find_symbol(right->name)->address);
    program.emplace_back("JZERO", 2);
}

void ConditionNode::compile_neq() {
    if (left->name == "" && right->name == "") {
        if (left->value != right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->name == "" && right->name != "") {
        program.emplace_back("SET", left->value);
        program.emplace_back("SUB", find_symbol(right->name)->address);
        program.emplace_back("JZERO", 2);
        program.emplace_back("JUMP", 2);
        return;
    }
    
    if (left->name != "" && right->name == "") {
        program.emplace_back("SET", right->value);
        program.emplace_back("SUB", find_symbol(left->name)->address);
        program.emplace_back("JZERO", 2);
        program.emplace_back("JUMP", 2);
        return;
    }
    
    program.emplace_back("LOAD", find_symbol(left->name)->address);
    program.emplace_back("SUB", find_symbol(right->name)->address);
    program.emplace_back("JZERO", 2);
    program.emplace_back("JUMP", 2);
}

void ConditionNode::compile_gt() {
    if (left->name == "" && right->name == "") {
        if (left->value == right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->name == "" && right->name != "") {
        program.emplace_back("SET", left->value);
        program.emplace_back("SUB", find_symbol(right->name)->address);
        program.emplace_back("JPOS", 2);
        return;
    }
    
    if (left->name != "" && right->name == "") {
        program.emplace_back("SET", right->value);;
        program.emplace_back("SUB", find_symbol(left->name)->address);
        program.emplace_back("JPOS", 2);
        return;
    }
    
    program.emplace_back("LOAD ", find_symbol(left->name)->address);
    program.emplace_back("SUB ", find_symbol(right->name)->address);
    program.emplace_back("JPOS ", 2);
}

void ConditionNode::compile_lt() {
    if (left->name == "" && right->name == "") {
        if (left->value == right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->name == "" && right->name != "") {
        program.emplace_back("SET", left->value);
        program.emplace_back("SUB", find_symbol(right->name)->address);
        program.emplace_back("JNEG", 2);
        return;
    }
    
    if (left->name != "" && right->name == "") {
        program.emplace_back("SET", right->value);
        program.emplace_back("SUB", find_symbol(left->name)->address);
        program.emplace_back("JNEG", 2);
        return;
    }
    
    program.emplace_back("LOAD", find_symbol(left->name)->address);
    program.emplace_back("SUB", find_symbol(right->name)->address);
    program.emplace_back("JNEG", 2);
}

void ConditionNode::compile_geq() {
    if (left->name == "" && right->name == "") {
        if (left->value == right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->name == "" && right->name != "") {
        program.emplace_back("SET", left->value);
        program.emplace_back("SUB", find_symbol(right->name)->address);
        program.emplace_back("JPOS", 3);
        program.emplace_back("JZERO", 2);
        return;
    }
    
    if (left->name != "" && right->name == "") {
        program.emplace_back("SET", right->value);
        program.emplace_back("SUB",find_symbol(left->name)->address);
        program.emplace_back("JPOS", 3);
        program.emplace_back("JZERO", 2);
        return;
    }
    
    program.emplace_back("LOAD", find_symbol(left->name)->address);
    program.emplace_back("SUB", find_symbol(right->name)->address);
    program.emplace_back("JPOS", 3);
    program.emplace_back("JZERO", 2);
}

void ConditionNode::compile_leq() {
    if (left->name == "" && right->name == "") {
        if (left->value == right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->name == "" && right->name != "") {
        program.emplace_back("SET", left->value);
        program.emplace_back("SUB", find_symbol(right->name)->address);
        program.emplace_back("JNEG", 3);
        program.emplace_back("JZERO", 2);
        return;
    }
    
    if (left->name != "" && right->name == "") {
        program.emplace_back("SET", right->value);
        program.emplace_back("SUB", find_symbol(left->name)->address);
        program.emplace_back("JNEG", 3);
        program.emplace_back("JZERO", 2);
        return;
    }
    
    program.emplace_back("LOAD", find_symbol(left->name)->address);
    program.emplace_back("SUB", find_symbol(right->name)->address);
    program.emplace_back("JNEG", 3);
    program.emplace_back("JZERO", 2);
}