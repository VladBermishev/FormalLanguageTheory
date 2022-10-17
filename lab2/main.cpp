#include <fstream>
#include <iostream>
#include <filesystem>
#include <common/stream-extension.h>
#include <lab2/regex.h>

int main(int argc, char** argv){
    istream_extension input(std::cin.rdbuf());
    std::ifstream finput;
    if (argc == 2) {
        if (std::filesystem::exists(argv[1])) {
            finput.open(argv[1]);
            input.rdbuf(finput.rdbuf());
        } else {
            throw std::runtime_error("File wasn't found at given filepath");
        }
    }
}