#pragma once 

#include "Node.hpp"

class CommandNode : public Node {
public:
    enum CommandType { ASSIGN, IF_ELSE, IF, WHILE, REPEAT, FOR, FOR_REV, PROC_CALL, READ, WRITE };
    CommandType type;

    CommandNode() = default;
    void compile() override;

private:
    void compile_assign();
    void compile_if_else();
    void compile_if();
    void compile_while();
    void compile_repeat();
    void compile_for();
    void compile_for_rev();
    void compile_proc_call();
    void compile_read();
    void compile_write();
};