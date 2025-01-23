#include "CommandsNode.hpp"
#include "instructions.hpp"

void CommandsNode::compile() {
    for (auto& cmd : commands) {
        cmd->compile();
    }
}