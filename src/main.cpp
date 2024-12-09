#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "parser.tab.hpp"

extern FILE* yyin;
extern int yyparse();

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");

    if (!yyin) 
    {
        std::cerr << "Error: Could not open input file" << std::endl;
        return EXIT_FAILURE;
    }

    if (yyparse() == 0) 
    {
        std::cout << "Parsing successful" << std::endl;
    } 
    else 
    {
        std::cerr << "Parsing failed" << std::endl;
    }

    fclose(yyin);

    return 0;
}
