#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <common/stream-extension.h>
#include <common/cmdline_positional_parser.h>
#include <lab2/regex.h>
#include <lab2/rewrite-rules.h>
using namespace std::chrono_literals;

void setup_input_stream(std::ifstream& fin, istream_extension& in, const std::string& source) noexcept{
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
    description.add_options(ParserOption("Initial regex source", "stdin"),
                            ParserOption("Rewrite rules source","stdin"),
                            ParserOption("Rewriting timeout (in seconds)", "10"));
    CmdlinePositionalParser parser(argc, argv, description);
    const auto ini_regex_source = parser[1].as<std::string>();
    const auto rules_source = parser[2].as<std::string>();
    const auto timeout = std::chrono::seconds(std::stoi(parser[3].as<std::string>()));
    istream_extension ini_regex_input(std::cin.rdbuf()), rules_input(std::cin.rdbuf());
    std::ifstream ini_regex_finput, rules_finput;
    setup_input_stream(ini_regex_finput, ini_regex_input, ini_regex_source);
    setup_input_stream(rules_finput, rules_input, rules_source);

    Regex initial_regex;
    ini_regex_input.lstrip("\r\n\t ") >> initial_regex;
    RewriteRules rules(initial_regex.grammar([](const char c){
        return (Regex::is_symbol(c)) ? 1 : -1;
    }));
    while(!rules_input.eof()){
        Regex lhs, rhs;
        rules_input.lstrip("\r\n\t ") >> lhs;
        rules_input.ignore(stream_max_size, '=').lstrip("\r\n\t ") >> rhs;
        if(lhs.type() != Regex::node_type::EMPTY || rhs.type() != Regex::node_type::EMPTY )
            rules.push_back({std::move(lhs), std::move(rhs)});
    }
    std::atomic_bool is_cancellation_required = false;
    const auto thread_start_time = std::chrono::system_clock::now();
    std::thread t1([&initial_regex, &rules, &is_cancellation_required](){
        auto find_result = rules.find_rule(initial_regex);
        for(;find_result.first != rules.cend() && !is_cancellation_required.load(); find_result = rules.find_rule(initial_regex)){
            auto substitution_result = find_result.first->rhs();
            substitution_result.substitute(find_result.second.variables_substitution);
            *find_result.second.result = substitution_result;
        }
        is_cancellation_required.store(true);
    });
    while(std::chrono::system_clock::now() - thread_start_time < timeout && !is_cancellation_required.load())
        std::this_thread::sleep_for(10ms);
    is_cancellation_required.store(true);
    t1.join();
    std::cout << initial_regex;
    return 0;
}