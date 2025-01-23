#include "instructions.hpp"

#include "Node.hpp"
#include "ArgsNode.hpp"
#include "CommandsNode.hpp"
#include "CommandNode.hpp"
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
        
            outfile.close();
            std::cout << "Parsing result saved to output.txt\n";
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
