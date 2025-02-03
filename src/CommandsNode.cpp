#include "CommandsNode.hpp"
#include "instructions.hpp"

CommandsNode::~CommandsNode() {
    for (auto& cmd : commands) {
        delete cmd;
    }
}

void CommandsNode::preprocessing() {
    for (auto& cmd : commands) {
        cmd->preprocessing();
    }
}

void CommandsNode::compile() {
    for (auto& cmd : commands) {
        cmd->compile();
    }
}