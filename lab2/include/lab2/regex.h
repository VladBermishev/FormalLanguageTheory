#pragma once
#include <istream>
#include <functional>
#include <stack>
#include <common/fxtbits-set.h>
#include <lab1/grammar.h>

class Regex{
public:
    enum node_type{
        EMPTY = 0,
        UNARY,
        BINARY,
        SYMBOL,
    };
    inline static bool is_symbol(const char symbol){
        return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
    }
    inline static bool is_unary(const char symbol){ return symbol == '*'; }
    inline static bool is_concat(const char symbol){ return is_symbol(symbol) || symbol == '('; }
    inline static bool is_alt(const char symbol){ return symbol == '|'; }
    inline static bool is_bynary(const char symbol){ return is_alt(symbol) || is_concat(symbol); }

    class RegexFindResult{
    public:
        Regex* result = nullptr;
        const Regex* variables_substitution[std::numeric_limits<uint8_t>::max() + 1]{};
    };


private:
    node_type _type;
    char _data;
    Regex* _left_node = nullptr;
    Regex* _right_node = nullptr;
public:
    Regex(const node_type type = node_type::EMPTY, const char data = 0) noexcept: _type(type), _data(data){}
    Regex(const Regex& regex) noexcept{ *this = regex; }
    Regex(Regex&& regex) noexcept{
        *this = std::move(regex);
    }
    Regex& operator=(const Regex& regex) noexcept{
        if (this != &regex){
            this->clear();
            __copy_assign(regex);
        }
        return *this;
    }
    Regex& operator=(Regex&& regex) noexcept {
        if (this != &regex){
            this->clear();
            _type = regex._type;
            _data = regex._data;
            _left_node = regex._left_node;
            _right_node = regex._right_node;
            regex._left_node = nullptr;
            regex._right_node = nullptr;
        }
        return *this;
    }

    inline char data() const noexcept { return _data; }
    inline node_type type() const noexcept { return  _type; }
    inline const Regex* left_node() const noexcept { return _left_node; }
    inline const Regex* right_node() const noexcept { return _right_node; }

    void substitute(const Regex* (&substitution)[std::numeric_limits<uint8_t>::max() + 1]) noexcept{
        std::stack<Regex*> tree_pointers;
        tree_pointers.push(this);
        while (!tree_pointers.empty()){
            Regex* node_pointer = tree_pointers.top();
            tree_pointers.pop();
            if (substitution[node_pointer->_data])
                *node_pointer = *substitution[node_pointer->_data];
            if(node_pointer->_left_node)
                tree_pointers.push(node_pointer->_left_node);
            if (node_pointer->_right_node)
                tree_pointers.push(node_pointer->_right_node);
        }
    }

    RegexFindResult find(const Regex& regex, const char_bitset_type& variables) noexcept{
        RegexFindResult result;
        if(__equal_with_variables(regex,variables,result.variables_substitution)) {
            result.result = this;
            return result;
        }else{
            if(_left_node){
                if (auto res = _left_node->find(regex, variables); res.result != nullptr)
                    return res;
            }
            if(_right_node){
                if ( auto res = _right_node->find(regex, variables); res.result != nullptr)
                    return res;
            }
        }
        return {nullptr};
    }

    /* grammar accepts following function f: const char -> int:
     * f returns: < 0 if c is: constructor
     *            0            variable
     *            > 0          constant
     */
    Grammar grammar(std::function<int(const char)> f) const noexcept{
        Grammar result;
        std::stack<const Regex*> tree_pointers;
        tree_pointers.push(this);
        while (!tree_pointers.empty()){
            const Regex* node_pointer = tree_pointers.top();
            tree_pointers.pop();
            const int __res = f(node_pointer->_data);
            if( __res < 0)
                result.insert_constructor(node_pointer->_data);
            else if( __res == 0)
                result.insert_variable(node_pointer->_data);
            else
                result.insert_constant(node_pointer->_data);

            if(node_pointer->_left_node)
                tree_pointers.push(node_pointer->_left_node);
            if (node_pointer->_right_node)
                tree_pointers.push(node_pointer->_right_node);
        }
        return result;
    }

    void clear() noexcept{
        _type = node_type::EMPTY;
        _data = 0;
        delete _left_node;delete _right_node;
        _left_node = nullptr; _right_node = nullptr;
    }

    bool operator==(const Regex& regex) const noexcept{
        bool result = (_type == regex._type) && (_data == regex._data);
        if (result && _left_node != nullptr && regex._left_node != nullptr)
            result &= *_left_node == *regex._left_node;
        if (result && _right_node != nullptr && regex._right_node != nullptr)
            result &= *_right_node == *regex._right_node;
        return result;
    }

    bool operator!=(const Regex& regex) const noexcept{ return !(*this == regex); }
    friend std::ostream& operator<<(std::ostream& out, Regex& regex) noexcept{
        switch (regex._type) {
            case node_type::BINARY:{
                if(regex._left_node && regex._right_node) {
                    out << '(' << *regex._left_node;
                    if (regex._data != '.') out << regex._data;
                    out << *regex._right_node << ')';
                }
                break;
            }
            case node_type::UNARY:{
                if(regex._left_node){
                    if(regex._left_node->_type == node_type::SYMBOL)
                        out << *regex._left_node << regex._data;
                    else
                        out << '(' <<*regex._left_node << ')' << regex._data;
                }
                break;
            }
            case node_type::SYMBOL:{
                out << regex._data;
                break;
            }
        }
        return out;
    }

    friend istream_extension& operator>>(istream_extension& in, Regex& regex) noexcept {
        regex.clear();
        const int __next = in.peek();
        if(__next == '('){
            in.get();
            auto* lhs = new Regex();
            in >> *lhs;
            if(in.peek() == ')'){
                in.get();
                if(in.eof() || !is_unary(in.peek())){
                    regex = *lhs;
                    lhs->clear();
                    delete lhs;
                }else{
                    regex._left_node = lhs;
                    regex._type = node_type::UNARY;
                    in >> regex._data;
                }
            }else{
                regex._type = node_type::BINARY;
                regex._data = is_alt(in.peek()) ? '|' : '.';
                if (is_alt(in.peek())) in.get();
                regex._left_node = lhs;
                regex._right_node = new Regex();
                in >> *regex._right_node;
                in.get();
            }
        }else if(is_symbol(__next)){
            const char symbol = in.get();
            if(!in.eof() && is_unary(in.peek())){
                regex._type = node_type::UNARY;
                in >> regex._data;
                regex._left_node = new Regex(node_type::SYMBOL, symbol);
            }else{
                regex._type = node_type::SYMBOL;
                regex._data = symbol;
            }
        }
        return in;
    }

    ~Regex(){
        delete _left_node;
        delete _right_node;
    }
private:
    void __copy_assign(const Regex& regex){
        _type = regex._type;
        _data = regex._data;
        if (regex._left_node){
            _left_node = new Regex();
            _left_node->__copy_assign(*regex._left_node);
        }
        if (regex._right_node){
            _right_node = new Regex();
            _right_node->__copy_assign(*regex._right_node);
        }
    }
    bool __equal_with_variables(const Regex& regex, const char_bitset_type& variables,
                                const Regex* (&substitution)[std::numeric_limits<uint8_t>::max() + 1]){
        if (__glibc_unlikely(variables.empty()))
            return *this == regex;
        else{
            if(variables.contains(regex._data)){
                if(substitution[static_cast<uint8_t>(regex._data)] == nullptr){
                    substitution[static_cast<uint8_t>(regex._data)] = this;
                    return true;
                }else
                    return *this == *substitution[static_cast<uint8_t>(regex._data)];
            }else{
                bool result = (_type == regex._type) && (_data == regex._data);
                if (result && _left_node != nullptr && regex._left_node != nullptr)
                    result &= _left_node->__equal_with_variables(*regex._left_node, variables, substitution);
                if (result && _right_node != nullptr && regex._right_node != nullptr)
                    result &= _right_node->__equal_with_variables(*regex._right_node, variables, substitution);
                return result;
            }
        }
    }
};