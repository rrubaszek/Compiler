#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <utility>
#include "parser.tab.hpp"

extern FILE* yyin;
extern std::vector<instruction> program;
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

        std::ofstream outfile("output.txt");
        if (outfile.is_open()) {
            for (const auto& [command, value] : program) {
                outfile << command << " " << value << "\n";
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
