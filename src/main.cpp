#include "instructions.hpp"

#include "Node.hpp"
#include "ArgsNode.hpp"
#include "CommandsNode.hpp"
#include "CommandNode.hpp"
#include "ConditionNode.hpp"
#include "DeclarationsNode.hpp"
#include "ExpressionNode.hpp"
#include "IdentifierNode.hpp"
#include "MainNode.hpp"
#include "ValueNode.hpp"
#include "ProcedureNode.hpp"
#include "ProgramNode.hpp"

#include "parser.tab.hpp"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

extern FILE* yyin;
extern int yyparse();

void preprocessing() {
    first_pass = true;
    if (yyparse() == 0) {
        std::cerr << "Preprocessing successful\n";
    }
    else {
        std::cerr << "Preprocessing failed\n";
        exit(1);
    }

    mark_called_procs("main");
    remove_unused_procs(); 
    rewind(yyin);
    first_pass = false;
    is_local = false;
    next_free_register = 11;
    next_temp_free_register = 1;
    program.clear();
    global_symbol_table.clear();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");

    if (!yyin) {
        std::cerr << "Error: Could not open input file" << std::endl;
        return EXIT_FAILURE;
    }

    preprocessing();
    
    if (yyparse() == 0) {
        std::cout << "Parsing successful" << std::endl;

        std::ofstream outfile("output.mr");
        if (outfile.is_open()) {
            for (const auto& [command, value] : program) { // TODO: maybe check if can be improved/cleaned-up
                outfile << command << " ";
                if (value.has_value()) 
                    outfile << value.value();
                outfile << "\n";
            }
        
            outfile << "HALT";
            outfile.close();
            std::cout << "Parsing result saved to output.mr\n";
        } 
        else {
            std::cerr << "Error: Could not open file for writing\n";
        }
    } 
    else {
        std::cerr << "Parsing failed" << std::endl;
    }

    fclose(yyin);

    return 0;
}
