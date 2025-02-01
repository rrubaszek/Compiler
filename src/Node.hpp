#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>

#include "instructions.hpp"

class Node {
public:
    virtual ~Node() {}
    virtual void compile() = 0;

    int lineno;
};