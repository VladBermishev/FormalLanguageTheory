#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <common/fxtbits-set.h>
#include <common/string-extension.h>
#include <common/stream-extension.h>
#include <common/perf.h>

class Grammar{
    using set_type = FxtBitsSet<256>;
    set_type _constructors;
    set_type _variables;
public:
    Grammar() = default;
    [[nodiscard]] inline bool is_constructor(const char c) const noexcept{ return _constructors.contains(c); }
    [[nodiscard]] inline bool is_variable(const char c) const noexcept{ return _variables.contains(c); }
    std::istream& parse_constructors(std::istream& in){
        std::string line; std::getline(in,line);
        for (const auto var: split(line.c_str(), line.size(),',')){
            _constructors.insert(*var.data());
        }
        return in;
    }
    std::istream& parse_variables(std::istream& in){
        std::string line; std::getline(in,line);
        for (const auto var: split(line.c_str(), line.size(),',')){
            _variables.insert(*var.data());
        }
        return in;
    }
};

class Term{
    char _name;
    std::vector<Term*> _children;
public:
    Term(): _name(0){}
    friend std::istream& operator>>(std::istream& in, Term& term){
        in >> term._name;
        if (in.peek() == '('){
            int __c = in.get();
            while (__c != ')'){
                term._children.push_back(new Term());
                in >> *term._children.back();
                __c = in.get();
            }
        }
        return in;
    }
    friend std::ostream& operator<<(std::ostream& out, const Term& term) noexcept{
        out << term._name;
        if ( !term._children.empty() ){
            out << '(';
            for(std::uint32_t idx = 0; idx < term._children.size() - 1; idx++ ){
                out << *term._children[idx] << ',';
            }
            out << *term._children.back() << ')';
        }
        return out;
    }
    ~Term(){
        for(const auto& tptr: _children){ delete tptr; }
    }
};

int main(int argc, char** argv){
    PERF("main")
    istream_extension input(std::cin.rdbuf());
    std::ifstream finput;
    if( argc == 2 ){
        if ( std::filesystem::exists(argv[1]) ){
            finput.open(argv[1]);
            input.rdbuf(finput.rdbuf());
        }else{
            throw std::runtime_error("File wasn't found at given filepath");
        }
    }
    Grammar grammar;
    input.lstrip("\r\n\t ").ignore(stream_max_size,'=').lstrip(" ");
    grammar.parse_constructors(input);
    input.lstrip("\r\n\t ").ignore(stream_max_size, '=').lstrip(" ");
    grammar.parse_variables(input);
    Term first_term;
    input.lstrip("\r\n\t ").ignore(stream_max_size, ':').lstrip(" ") >> first_term;
    Term second_term;
    input.lstrip("\r\n\t ").ignore(stream_max_size,':').lstrip(" ") >> second_term;
    std::cout << first_term << '\n' << second_term << '\n';
    return 0;
}