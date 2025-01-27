#include "ConditionNode.hpp"
#include "instructions.hpp"

void ConditionNode::compile()  {
    std::cout << type << "\n";
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
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (left->value == right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JZERO", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JZERO", 2);
            return;
        }
    }
    
    if (right->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JZERO", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JZERO", 2);
            return;
        }
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    program.emplace_back("JZERO", 2);
    free_register(temp);    
}

void ConditionNode::compile_neq() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (left->value != right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JZERO", 2);
            program.emplace_back("JUMP", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JZERO", 2);
            program.emplace_back("JUMP", 2);
            return;
        }
    }
    
    if (right->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JZERO", 2);
            program.emplace_back("JUMP", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JZERO", 2);
            program.emplace_back("JUMP", 2);
            return;
        }
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    program.emplace_back("JZERO", 2);
    program.emplace_back("JUMP", 2);
    free_register(temp);
}

void ConditionNode::compile_gt() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (left->value > right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->type == ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JPOS", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JPOS", 2);
            return;
        }
    }
    
    if (right->type == ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JNEG", 2);
            return;
        }

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JNEG", 2);
            return;
        }
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    program.emplace_back("JPOS", 2);
    free_register(temp); 
}

void ConditionNode::compile_lt() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (left->value < right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JNEG", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JNEG", 2);
            return;
        }
    }
    
    if (right->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JPOS", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JPOS", 2);
            return;
        }
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    program.emplace_back("JNEG", 2);
    free_register(temp); 
}

void ConditionNode::compile_geq() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (left->value >= right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JPOS", 3);
            program.emplace_back("JZERO", 2);
            return; 
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JPOS", 3);
            program.emplace_back("JZERO", 2);
            return; 
        }

    }
    
    if (right->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JPOS", 3);
            program.emplace_back("JZERO", 2);
            return; 
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JPOS", 3);
            program.emplace_back("JZERO", 2);
            return; 
        }
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    program.emplace_back("JPOS", 3);
    program.emplace_back("JZERO", 2);
    free_register(temp);
}

void ConditionNode::compile_leq() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (left->value <= right->value) {
            res = TRUE;
        }
        else {
            res = FALSE;
        }
        return;
    }

    if (left->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JNEG", 3);
            program.emplace_back("JZERO", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JNEG", 3);
            program.emplace_back("JZERO", 2);
            return;
        }
        
    }
    
    if (right->type ==  ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUB", symbol->address);
            program.emplace_back("JNEG", 3);
            program.emplace_back("JZERO", 2);
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", right->value);
            program.emplace_back("SUBI", symbol->address);
            program.emplace_back("JNEG", 3);
            program.emplace_back("JZERO", 2);
            return;
        }
    }

    right->compile();
    int temp = allocate_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    program.emplace_back("JNEG", 3);
    program.emplace_back("JZERO", 2);
    free_register(temp);
}