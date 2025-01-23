#include "CommandsNode.hpp"

void CommandsNode::compile() {
    for (auto& cmd : commands) {
        cmd->compile();
    }
}