#include "MainNode.hpp"

void MainNode::compile()  {
    // Kod kompilacji dla głównego programu
    std::cout << "Compiling main program.\n";
    declarations->compile();
    commands->compile();
}