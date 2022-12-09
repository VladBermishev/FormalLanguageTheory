#pragma once
#include <stack>
#include <vector>
#include <string>
#include <algorithm>

class Regex{
public:
    enum node_type{
        EMPTY = 0,
        NONTERMINAL,
        TERMINAL,
        ALTERNATIVE,
        CONCAT,
        STAR,
        POWER
    };
private:
    node_type _type;
    std::string _data;
    std::vector<Regex*> _children;
public:
    Regex(const node_type type = EMPTY, const std::string& data = "") noexcept: _type(type), _data(data){}
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
            _children = std::move(regex._children);
        }
        return *this;
    }

    inline auto& node_type() noexcept { return _type; }
    inline const auto& node_type() const noexcept { return _type; }
    inline const std::string& data() const noexcept { return _data; }
    inline std::string& data() noexcept { return _data; }
    inline std::vector<Regex*>& children() noexcept { return _children; }
    inline const std::vector<Regex*>& children() const noexcept { return _children; }

    void push_back(const Regex& regex){
        _children.push_back(new Regex());
        *_children.back() = regex;
    }

    void push_back(Regex&& regex){
        _children.push_back(new Regex());
        *_children.back() = std::move(regex);
    }

    void substitute(const GrammarUnit& nonterminal, const Regex& regex) noexcept{
        std::stack<Regex*> tree_pointers;
        tree_pointers.push(this);
        while (!tree_pointers.empty()){
            Regex* node_pointer = tree_pointers.top();
            tree_pointers.pop();
            if( node_pointer->_type == NONTERMINAL && node_pointer->_data == nonterminal.data()){
                *node_pointer = regex;
            }else {
                for(const auto& node_ptr: node_pointer->_children)
                    if (node_ptr) tree_pointers.push(node_ptr);
            }
        }
    }

    void release_brackets(){
        if(_type == CONCAT){
            auto it = std::find_if(_children.begin(),_children.end(),
                                   [](const Regex* ptr){ return ptr->_type == CONCAT; });
            while( it != _children.end()){
                for(const auto node_ptr: (*it)->_children)
                    _children.push_back(node_ptr);

                (*it)->_children.clear();
                delete (*it);
                _children.erase(it);

                it = std::find_if(_children.begin(),_children.end(),
                                  [](const Regex* ptr){ return ptr->_type == CONCAT; });
            }
        }
        for (auto& node_ptr: _children)
            node_ptr->release_brackets();
    }

    void reorder( const GrammarUnit& variable ){
        if(_type == CONCAT){
            auto metric = [&variable](const Regex& regex)-> std::uint8_t {
                if( (regex._type == NONTERMINAL && regex._data == variable.data()) ||
                    (regex._type == POWER && !regex._children.empty() && regex._children[0]->_data == variable.data()))
                    return 3;
                if(regex._type == TERMINAL)
                    return 2;
                if(regex._type == NONTERMINAL)
                    return 1;
                return 0;
            };
            auto cmp = [&metric](Regex* lhs, Regex* rhs){
                const std::uint8_t lhs_metric = metric(*lhs), rhs_metric = metric(*rhs);
                if(lhs_metric == rhs_metric)
                    return Regex::__reorder_less(*lhs, *rhs);
                else
                    return lhs_metric < rhs_metric;
            };
            std::sort(_children.begin(), _children.end(), cmp);
        }
        for (auto& node_ptr: _children)
            node_ptr->reorder(variable);
    }

    /* !!!! only after reorder !!!! */
    void compactify_on_powers(){
        if(_type == CONCAT){
            std::vector<Regex*> compact;
            std::vector<bool> visited(_children.size(), false);
            for(std::uint32_t i = 0; i < _children.size(); i++){
                if(!visited[i]) {
                    visited[i] = true;
                    std::uint32_t initial_power = _children[i]->_type == POWER ? std::stoi(_children[i]->_data) : 1;
                    std::uint32_t power = initial_power;
                    for (std::uint32_t j = i + 1; j < _children.size(); j++)
                        if (!visited[j] && __compactify_eq(*_children[i], *_children[j])) {
                                visited[j] = true;
                                power += _children[j]->_type == POWER ? std::stoi(_children[j]->_data) : 1;
                        }
                    if(power == initial_power){
                        compact.push_back(_children[i]);
                    }else if (power > initial_power){
                        Regex buffer(POWER, std::to_string(power));
                        buffer.push_back(*_children[i]);
                        compact.push_back(new Regex());
                        *compact.back() = std::move(buffer);
                    }
                }
            }
            _children = compact;
        }
        for (auto& node_ptr: _children)
            node_ptr->compactify_on_powers();
    }

    bool is_kleeny_star_problem(const GrammarUnit& variable) const noexcept{
        return false;
    }
    void iterate_kleene( const GrammarUnit& variable ){}

    template<typename F>
    bool contains_if(F f) const noexcept{
        bool result = f(*this);
        for(std::uint32_t idx = 0; idx < _children.size() && !result; ++idx)
            result |= _children[idx]->contains_if(f);
        return result;
    }

    void clear() noexcept{
        _type = node_type::EMPTY;
        _data.clear();
        for( auto& ptr: _children ){
            delete ptr;
            ptr = nullptr;
        }
    }

    bool operator==(const Regex& regex) const noexcept{
        bool result = (_type == regex._type) && (_data == regex._data);
        result &= _children.size() == regex._children.size();
        for( std::uint32_t idx = 0; idx < _children.size() && result; ++idx )
            if ( _children[idx] != nullptr && regex._children[idx] != nullptr )
                result &= *_children[idx] == *regex._children[idx];
        return result;
    }

    bool operator!=(const Regex& regex) const noexcept{ return !(*this == regex); }

    friend std::ostream& operator<<(std::ostream& out, const Regex& regex){
        return out;
    }

    ~Regex(){
        for( auto& ptr: _children ) delete ptr;
    }

private:
    void __copy_assign(const Regex& regex){
        _type = regex._type;
        _data = regex._data;
        for(const auto node_ptr: regex._children )
            if(node_ptr){
                _children.push_back(new Regex());
                _children.back()->__copy_assign(*node_ptr);
            }
    }
    static bool __reorder_less(const Regex& lhs, const Regex& rhs) noexcept{ return lhs.data() < rhs.data(); }
    static bool __compactify_eq(const Regex& lhs, const Regex& rhs) noexcept{
        const Regex* __lhs = &lhs;
        const Regex* __rhs = &rhs;
        if(lhs._type == POWER)
            __lhs = (!lhs._children.empty() ? lhs._children[0] : nullptr);
        if(rhs._type == POWER)
            __rhs = (!rhs._children.empty() ? rhs._children[0] : nullptr);
        return __lhs && __rhs && *__lhs == *__rhs;
    }
};