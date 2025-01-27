#include "IdentifierNode.hpp"
#include "instructions.hpp"

void IdentifierNode::compile()  {
    auto symbol = find_symbol(name);
    if (symbol->type == POINTER) {
        is_pointer = true;
    }
}