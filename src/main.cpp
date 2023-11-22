#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "generator.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Invalid usage. Correct usage...\n"
                  << "ntc <input.nt> <output.asm>\n";
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::ifstream input(argv[1]);
        std::stringstream contents_stream;
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);
    auto tokens = tokenizer.tokenize();

    Parser parser(tokens);
    auto prog = parser.parse_prog();

    if (!prog.has_value()) {
        std::cerr << "ERROR: could not parse source code\n";
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    generator.generate_prog();
    auto asm_string = generator.get_string();

    {
        std::ofstream output(argv[2]);
        output << asm_string;
    }

    return EXIT_SUCCESS;
}
