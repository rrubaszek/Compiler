#pragma once 

#include "Node.hpp"

class DeclarationsNode : public Node {
public:
    struct ArrayDeclarations {
        std::string name; 
        int start;         
        int end;
    };

    std::vector<std::string> variables;
    std::vector<ArrayDeclarations> arrays;

    DeclarationsNode() = default;
    void compile() override;
};