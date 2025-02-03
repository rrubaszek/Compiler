#pragma once

#include <variant>

#include "ExpressionNode.hpp"
#include "ConditionNode.hpp"
#include "IdentifierNode.hpp"
#include "ValueNode.hpp"

class CommandsNode;

class CommandNode : public Node {
public:
    enum CommandType {
        ASSIGN, IF_ELSE, IF, WHILE, REPEAT,
        FOR, FOR_REV, PROC_CALL, READ, WRITE
    };

    CommandType type;


    struct AssignData {
        IdentifierNode* left;       
        ExpressionNode* right;     
    };

    struct IfElseData {
        ConditionNode* condition; 
        CommandsNode* then_branch;  
        CommandsNode* else_branch; 
    };

    struct IfData {
        ConditionNode* condition;
        CommandsNode* then_branch;  
    };

    struct WhileData {
        ConditionNode* condition;  
        CommandsNode* loop_body;    
    };

    struct RepeatData {
        CommandsNode* loop_body;    
        ConditionNode* condition; 
    };

    struct ForData {
        std::string loop_variable;
        ValueNode* start_value;        
        ValueNode* end_value;          
        CommandsNode* loop_body;       
    };

    struct ForRevData {
        std::string loop_variable;
        ValueNode* start_value;        
        ValueNode* end_value;          
        CommandsNode* loop_body;       
    };

    struct ProcCallData {
        std::string name;
        std::vector<std::string> args;
    };

    struct ReadData {
        IdentifierNode* target;        
    };

    struct WriteData {
        ValueNode* value;              
    };

    std::variant<
        AssignData,
        IfElseData,
        IfData,
        WhileData,
        RepeatData,
        ForData,
        ForRevData,
        ProcCallData,
        ReadData,
        WriteData
    > data;

    CommandNode() = default;
    ~CommandNode() = default;
    void compile() override;
    void preprocessing() override;

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