#pragma once
#include <istream>
#include <common/fxtbits-set.h>

class Regex{
public:
    enum node_type{
      BINARY = 0,
      UNARY,
      SYMBOLS,
      EMPTY
    };
    inline static bool is_symbol(const char symbol){
        return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
    }
    inline static bool is_unary(const char symbol){ return !is_symbol(symbol) && symbol == '*'; }
    inline static bool is_bynary(const char symbol){ return !is_symbol(symbol) && symbol == '|'; }
private:
    node_type _type;
    char_bitset_type _data;
    Regex* _left_node = nullptr;
    Regex* _right_node = nullptr;
public:
    Regex() = default;

    friend std::istream& operator>>(std::istream& in, Regex& regex) noexcept {}
    ~Regex(){ delete _left_node; delete _right_node; }
};