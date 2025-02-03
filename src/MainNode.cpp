#include "MainNode.hpp"
#include "instructions.hpp"

MainNode::~MainNode() {
    delete declarations;
    delete commands;
}

void MainNode::preprocessing() {
    std::vector<std::pair<std::string, bool>> args;
    add_procedure("main", 0, 0, false, args);
    called_procedures.push("main");

    if (commands != nullptr) {
        commands->preprocessing();
    }
}

void MainNode::compile()  {
   
    if (declarations != nullptr) {
        declarations->compile();
    }

    if (commands != nullptr) {
        commands->compile();
    }
}