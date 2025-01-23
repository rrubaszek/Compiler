#include "MainNode.hpp"
#include "instructions.hpp"

void MainNode::compile()  {
    // Kod kompilacji dla głównego programu
    std::cout << "Compiling main program.\n";
    if (declarations != nullptr) {
        declarations->compile();
    }

    if (commands != nullptr) {
        commands->compile();
    }
}