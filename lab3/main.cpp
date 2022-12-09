#include <filesystem>
#include <fstream>
#include <iostream>
#include <lab3/context_free_grammar.h>
#include <lab3/equations_system.h>
#include <common/cmdline_positional_parser.h>
#include <common/stream-extension.h>


void setup_input_stream(std::ifstream& fin, istream_extension& in, const std::string& source) {
    if (source != "stdin") {
        if (std::filesystem::exists(source)) {
            fin.open(source);
            in.rdbuf(fin.rdbuf());
        } else {
            throw std::runtime_error(std::string("File wasn't found at given filepath: ") + source);
        }
    }
}

int main(int argc, const char* argv[]){
    PositionalOptionsDescription description("Allowed options:");
    description.add_options(ParserOption("Grammar source ::= [\"filepath\" | stdin] ", "stdin"));
    CmdlinePositionalParser parser(argc, argv, description);
    istream_extension input(std::cin.rdbuf());
    std::ifstream finput;
    setup_input_stream(finput, input, parser[1].as<std::string>());

    ContextFreeGrammar grammar;
    input >> grammar;
    const auto tmp = EquationsSystem(grammar.remove_epsilon_rules().remove_chain_rules()).commutative_image();
    for(const auto& nterm_comm_image: tmp)
        std::cout << nterm_comm_image << '\n';
    return 0;
}