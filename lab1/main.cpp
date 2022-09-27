#include <cstdint>
#include <iostream>
#include <fstream>
#include <common/fxtbits.h>
#include <common/fxtbits-set.h>
#include <common/string-extension.h>
#include <filesystem>

class Constructor{
    const char _name;
    const std::uint32_t _params_len;
public:
    Constructor() = delete;
    explicit Constructor(const char name, const std::uint32_t params_len) noexcept : _name(name), _params_len(params_len){}
    [[nodiscard]] inline char name() const noexcept{ return _name; }
    [[nodiscard]] inline std::uint32_t params_len() const noexcept { return _params_len; }
    ~Constructor() = default;
};

class Variable{
    const char _name;
public:
    Variable() = delete;
    explicit Variable( char name) noexcept: _name(name){}
    [[nodiscard]] inline char name() const noexcept { return _name; }
    ~Variable() = default;
};

class Term{
    const char _name;
    std::vector<Term*> _children;
public:
    Term(): _name(0){}
    friend std::istream& operator>>(std::istream& in, Term& term){
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
};

int main(int argc, char** argv){
    std::istream input(std::cin.rdbuf());
    std::ifstream finput;
    if( argc == 2 ){
        if ( std::filesystem::exists(argv[1]) ){
            finput.open(argv[1]);
            input.rdbuf(finput.rdbuf());
        }else{
            throw std::runtime_error("File wasn't found at given filepath");
        }
    }

    return 0;
}