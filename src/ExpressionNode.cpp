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

    if (left->type == ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("ADD", symbol->address); 
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("ADDI", symbol->address); 
            return;
        }
    }

    if (right->type == ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", right->value);
            program.emplace_back("ADD", symbol->address); 
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", right->value);
            program.emplace_back("ADDI", symbol->address); 
            return;
        }
    }

    // TODO: maybe this can be shortened
    right->compile();
    int temp = allocate_temp_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("ADD", temp);
    free_temp_register(temp);
}

void ExpressionNode::compile_sub() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        program.emplace_back("SET", left->value - right->value);
        return;
    }

    if (left->type == ValueNode::ValueType::CONSTANT) {
        auto symbol = find_symbol(right->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUB", symbol->address); 
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", left->value);
            program.emplace_back("SUBI", symbol->address); 
            return;
        }
    }

    if (right->type == ValueNode::ValueType::CONSTANT) {
       auto symbol = find_symbol(left->name);

        if (symbol->type == Type::VARIABLE) {
            program.emplace_back("SET", -(right->value));
            program.emplace_back("ADD", symbol->address); 
            return;
        }

        if (symbol->type == Type::POINTER) {
            program.emplace_back("SET", -(right->value));
            program.emplace_back("ADDI", symbol->address); 
            return;
        }
    }

    right->compile();
    int temp = allocate_temp_register();
    program.emplace_back("STORE", temp);
    left->compile();
    program.emplace_back("SUB", temp);
    free_temp_register(temp);

}
void ExpressionNode::compile_mul() {
    if (left->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        program.emplace_back("SET", right->value * left->value); // TODO: Fix constant expressions if they go out of range
        return;
    }

    // if (left->type == ValueNode::ValueType::OTHER && right->value < 10 && right->value > 0) {
    //     left->compile();
    //     for (int i = 1; i < right->value; i++) {
    //         program.emplace_back("ADD", program[program.size()-1].operand);
    //     }
    //     return;
    // }

    // if (left->type == ValueNode::ValueType::ARRAY_ELEMENT && right->value < 10 && right->value > 0) {
    //     left->compile();
    //     for (int i = 1; i < right->value; i++) {
    //         program.emplace_back("ADDI", program[program.size()-1].operand);
    //     }
    //     return;
    // }

    // if (right->type != ValueNode::ValueType::ARRAY_ELEMENT && left->value < 10 && left->value > 0) {
    //     right->compile();
    //     for (int i = 1; i < left->value; i++) {
    //         program.emplace_back("ADD", program[program.size()-1].operand);
    //     }
    //     return;
    // }

    // if (right->type == ValueNode::ValueType::ARRAY_ELEMENT && left->value < 10 && left->value > 0) {
    //     right->compile();
    //     for (int i = 1; i < left->value; i++) {
    //         program.emplace_back("ADDI", program[program.size()-1].operand);
    //     }
    //     return;
    // }

    int a_addr = allocate_temp_register();
    int b_addr = allocate_temp_register();

    left->compile();
    program.emplace_back("STORE", a_addr);

    right->compile();
    program.emplace_back("STORE", b_addr);

    int result_addr = allocate_temp_register(); 
      
    program.emplace_back("SET", 0);        
    program.emplace_back("STORE", result_addr);

    // Check if a or b are negative
    int sign_addr = allocate_temp_register();

    program.emplace_back("SET", 1);
    program.emplace_back("STORE", sign_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JPOS", 6);       
    program.emplace_back("SET", -1);      
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);       
    program.emplace_back("SUB", b_addr);
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("JPOS", 7);       
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", sign_addr); 
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);      
    program.emplace_back("SUB", a_addr);
    program.emplace_back("STORE", a_addr);

    // Main multiplication loop
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JZERO", 18);     // Exit if b == 0

    int check_parity = allocate_temp_register();

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", check_parity); 
    program.emplace_back("LOAD", check_parity);
    program.emplace_back("ADD", check_parity);
    program.emplace_back("SUB", b_addr);
    program.emplace_back("JZERO", 4); // If b is even don't add a to the result

    free_temp_register(check_parity);

    // Add a to the result
    program.emplace_back("LOAD", result_addr);
    program.emplace_back("ADD", a_addr); 
    program.emplace_back("STORE", result_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("ADD", a_addr);
    program.emplace_back("STORE", a_addr);

    program.emplace_back("JUMP", -18); // Back to the main loop

    // Make result with right sign
    program.emplace_back("LOAD", sign_addr);
    program.emplace_back("JPOS", 4);       
    program.emplace_back("SET", 0);        
    program.emplace_back("SUB", result_addr);
    program.emplace_back("STORE", result_addr);
    program.emplace_back("LOAD", result_addr);

    free_temp_register(sign_addr);
    free_temp_register(result_addr);
    free_temp_register(a_addr);
    free_temp_register(b_addr);
}
void ExpressionNode::compile_div() {
    if (right->value == 2) {
        left->compile();
        program.emplace_back("HALF");
        return;
    }

    // Entities has value only for constants
    if (right->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
        if (right->value < 0)
            program.emplace_back("SET", right->value / left->value + 1);
        else 
            program.emplace_back("SET", right->value / left->value);
        return;
    }

    int a_addr = allocate_temp_register();
    int b_addr = allocate_temp_register();

    left->compile();
    program.emplace_back("STORE", a_addr);

    right->compile();
    program.emplace_back("STORE", b_addr);

    int quotient_addr = allocate_temp_register(); 
    int remainder_addr = allocate_temp_register(); 
    int temp_addr = allocate_temp_register();      
    int sign_addr = allocate_temp_register();    
      
    program.emplace_back("STORE", sign_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JPOS", 6);       
    program.emplace_back("SET", -1);      
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);       
    program.emplace_back("SUB", b_addr);
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("JPOS", 7);      
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", sign_addr); 
    program.emplace_back("STORE", sign_addr);
    program.emplace_back("SET", 0);      
    program.emplace_back("SUB", a_addr);
    program.emplace_back("STORE", a_addr);
    program.emplace_back("STORE", remainder_addr);

    program.emplace_back("SET", 0);    
    program.emplace_back("STORE", quotient_addr);

    // Find largest b multiply
    program.emplace_back("SET", 1);
    program.emplace_back("STORE", temp_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("ADD", b_addr);
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("ADD", temp_addr);
    program.emplace_back("STORE", temp_addr);
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("SUB", a_addr);
    program.emplace_back("JPOS", 2);
    program.emplace_back("JUMP", -9);

    // Redo b and temp to the previous values
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", temp_addr);

    // Main loop
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("JZERO", 15);

    program.emplace_back("LOAD", remainder_addr);
    program.emplace_back("SUB", b_addr);
    program.emplace_back("JNEG", 5);
    program.emplace_back("STORE", remainder_addr);

    program.emplace_back("LOAD", quotient_addr);
    program.emplace_back("ADD", temp_addr);
    program.emplace_back("STORE", quotient_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", temp_addr);
    program.emplace_back("JUMP", -15);

    // Set the right sign
    program.emplace_back("LOAD", sign_addr);
    program.emplace_back("JPOS", 4);
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", quotient_addr);
    program.emplace_back("STORE", quotient_addr);

    // Set the right reminder
    program.emplace_back("LOAD", remainder_addr);
    program.emplace_back("JZERO", 6);
    program.emplace_back("LOAD", sign_addr);
    program.emplace_back("JPOS", 4);
    program.emplace_back("SET", -1);
    program.emplace_back("ADD", quotient_addr);
    program.emplace_back("STORE", quotient_addr);

    program.emplace_back("LOAD", quotient_addr);

    free_temp_register(temp_addr);
    free_temp_register(sign_addr);
    free_temp_register(quotient_addr);
    free_temp_register(remainder_addr);
    free_temp_register(a_addr);
    free_temp_register(b_addr);
}
void ExpressionNode::compile_mod() {
    // if (right->type == ValueNode::ValueType::CONSTANT && right->type == ValueNode::ValueType::CONSTANT) {
    //     int mod = std::abs(right->value) % std::abs(left->value);
    //     if (right->value < 0)
    //         program.emplace_back("SET", mod - right->value);
    //     else 
    //         program.emplace_back("SET", mod);
    //     return;
    // }

    int a_addr = allocate_temp_register();
    int b_addr = allocate_temp_register();
    int fix = allocate_temp_register();

    left->compile();
    program.emplace_back("STORE", a_addr);

    right->compile();
    program.emplace_back("STORE", b_addr);
    program.emplace_back("STORE", fix);

    int remainder_addr = allocate_temp_register(); 
    int temp_addr = allocate_temp_register();      
    int sign_a = allocate_temp_register(); 
    int sign_b = allocate_temp_register();  

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("JPOS", 6);       
    program.emplace_back("SET", -1);      
    program.emplace_back("STORE", sign_b);
    program.emplace_back("SET", 0);       
    program.emplace_back("SUB", b_addr);
    program.emplace_back("STORE", b_addr);

    program.emplace_back("LOAD", a_addr);
    program.emplace_back("JPOS", 6);      
    program.emplace_back("SET", -1); 
    program.emplace_back("STORE", sign_a);
    program.emplace_back("SET", 0); 
    program.emplace_back("SUB", a_addr);
    program.emplace_back("STORE", a_addr);
    program.emplace_back("STORE", remainder_addr);

    // Find largest b multiply
    program.emplace_back("SET", 1);
    program.emplace_back("STORE", temp_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("ADD", b_addr);
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("ADD", temp_addr);
    program.emplace_back("STORE", temp_addr);
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("SUB", a_addr);
    program.emplace_back("JPOS", 2);
    program.emplace_back("JUMP", -9);

    // Redo b and temp to the previous values
    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", temp_addr);

    // Main loop
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("JZERO", 12);

    program.emplace_back("LOAD", remainder_addr);
    program.emplace_back("SUB", b_addr);
    program.emplace_back("JNEG", 2);
    program.emplace_back("STORE", remainder_addr);

    program.emplace_back("LOAD", b_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", b_addr);
    program.emplace_back("LOAD", temp_addr);
    program.emplace_back("HALF");
    program.emplace_back("STORE", temp_addr);
    program.emplace_back("JUMP", -12);


    // TODO: fix this stupid modulo operation
    // Set the right reminder
    program.emplace_back("LOAD", remainder_addr);
    program.emplace_back("JZERO", 16);
    program.emplace_back("LOAD", sign_a);
    program.emplace_back("JZERO", 9);
    program.emplace_back("LOAD", sign_b);
    program.emplace_back("JZERO", 4);
    // Both negative
    program.emplace_back("SET", 0);
    program.emplace_back("SUB", remainder_addr);
    program.emplace_back("JUMP", 8);
    // b positive
    program.emplace_back("LOAD", fix);
    program.emplace_back("SUB", remainder_addr);
    program.emplace_back("JUMP", 5);
    // a positive
    program.emplace_back("LOAD", sign_b);
    program.emplace_back("JZERO", 4);
    program.emplace_back("LOAD", remainder_addr);
    program.emplace_back("ADD", fix);
    
    // both positive
    program.emplace_back("STORE", remainder_addr);
    program.emplace_back("LOAD", remainder_addr);

    free_temp_register(temp_addr);
    free_temp_register(sign_a);
    free_temp_register(sign_b);
    free_temp_register(remainder_addr);
    free_temp_register(a_addr);
    free_temp_register(b_addr);
    free_temp_register(fix);
}