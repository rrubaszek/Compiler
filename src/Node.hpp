#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class Node {
public:
    std::string name;

    virtual ~Node() {}
    virtual void compile() = 0;
};