#include "ValueNode.hpp"
#include "instructions.hpp"

void ValueNode::compile() {
    if (value)
        std::cout << "Compiling number: " << value << "\n";
    
    if (name != "")
        std::cout << "Compiling variable: " << name << "\n";
}