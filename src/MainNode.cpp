#include "MainNode.hpp"
#include "instructions.hpp"

void MainNode::compile()  {
    if (first_pass) {
        std::vector<std::pair<std::string, bool>> args;
        add_procedure("main", 0, 0, false, args);
        called_procedures.push("main");
    }
    
    if (declarations != nullptr) {
        declarations->compile();
    }

    if (commands != nullptr) {
        commands->compile();
    }
}