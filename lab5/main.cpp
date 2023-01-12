#include <filesystem>
#include <fstream>
#include <iostream>
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
    return 0;
}