#pragma once
#include <lab3/grammar_unit.h>
#include <common/stream-extension.h>
#include <stack>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>

class CommutativeRegex{
public:
    enum node_type{
        EMPTY = 0,
        NONTERMINAL,
        TERMINAL,
        ALTERNATIVE,
        CONCAT,
        STAR,
    };
private:
    node_type _type;
    std::string _data;
    std::uint32_t _power;
    std::vector<CommutativeRegex*> _children;
public:

    CommutativeRegex(const node_type type = EMPTY, std::string  data = "", const std::uint32_t power = 1) noexcept :
    _type(type), _data(std::move(data)), _power(power)
    {}

    CommutativeRegex(const CommutativeRegex& regex) noexcept{ *this = regex; }

    CommutativeRegex(CommutativeRegex&& regex) noexcept{
        *this = std::move(regex);
    }

    CommutativeRegex& operator=(const CommutativeRegex& regex) noexcept{
        if (this != &regex){
            this->clear();
            __copy_assign(regex);
        }
        return *this;
    }

    CommutativeRegex& operator=(CommutativeRegex&& regex) noexcept {
        if (this != &regex){
            this->clear();
            _type = regex._type;
            _data = regex._data;
            _power = regex._power;
            _children = std::move(regex._children);
        }
        return *this;
    }

    inline auto& node_type() noexcept { return _type; }
    inline const auto& node_type() const noexcept { return _type; }
    inline const std::string& data() const noexcept { return _data; }
    inline std::string& data() noexcept { return _data; }
    inline const std::uint32_t& power() const noexcept { return _power; }
    inline std::uint32_t& power() noexcept { return _power; }
    inline std::vector<CommutativeRegex*>& children() noexcept { return _children; }
    inline const std::vector<CommutativeRegex*>& children() const noexcept { return _children; }

    void push_back(const CommutativeRegex& regex){
        _children.push_back(new CommutativeRegex(regex));
    }

    void push_back(CommutativeRegex&& regex){
        _children.push_back(new CommutativeRegex(std::move(regex)));
    }

    void substitute(const GrammarUnit& nonterminal, const CommutativeRegex& regex) noexcept{
        std::stack<CommutativeRegex*> tree_pointers;
        tree_pointers.push(this);
        while (!tree_pointers.empty()){
            CommutativeRegex* node_pointer = tree_pointers.top();
            tree_pointers.pop();
            if( node_pointer->_type == NONTERMINAL && node_pointer->_data == nonterminal.data()){
                const auto __tmp_power = node_pointer->power();
                *node_pointer = regex;
                node_pointer->power() = __tmp_power;
            }else {
                for(const auto& node_ptr: node_pointer->_children)
                    if (node_ptr) tree_pointers.push(node_ptr);
            }
        }
    }

    std::pair<CommutativeRegex, CommutativeRegex> distribute_variable(const GrammarUnit& variable){
        CommutativeRegex independent_term, dependent_term;
        if(_type == ALTERNATIVE){
            independent_term = CommutativeRegex(ALTERNATIVE, "+");
            dependent_term = CommutativeRegex(ALTERNATIVE, "+");
            for(const auto& child: _children){
                auto [new_independent, new_dependent] = child->distribute_variable(variable);
                if (new_independent._type != EMPTY)
                    independent_term.push_back(std::move(new_independent));
                if (new_dependent._type != EMPTY)
                    dependent_term.push_back(std::move(new_dependent));
            }
            if(independent_term._children.empty())
                independent_term = CommutativeRegex();
            if(independent_term._children.size() == 1){
                const auto buffer = *independent_term._children[0];
                independent_term = buffer;
            }
            if(dependent_term._children.empty())
                dependent_term = CommutativeRegex();
            if(dependent_term._children.size() == 1){
                const auto buffer = *dependent_term._children[0];
                dependent_term = buffer;
            }
        }else if(_type == CONCAT){
            const std::uint32_t power = __terms_power(variable);
            if(power != 0){
                dependent_term = *this;
            }else{
                CommutativeRegex independent_tail(CONCAT, ".");
                CommutativeRegex* Kleene_problem = nullptr;
                for(const auto& child: _children) {
                    if (child->contains(variable.data()))
                        Kleene_problem = Kleene_problem == nullptr ? child : Kleene_problem;
                    else
                        independent_tail.push_back(*child);
                }
                if(!Kleene_problem){
                    independent_term = independent_tail;
                }else{
                    auto [new_independent, new_dependent] = Kleene_problem->distribute_variable(variable);
                    independent_term = CommutativeRegex(CONCAT, ".");
                    independent_term.push_back(std::move(new_independent));
                    independent_term.push_back(independent_tail);
                    dependent_term = CommutativeRegex(CONCAT, ".");
                    for(const auto& child: _children) {
                        if(child != Kleene_problem)
                            dependent_term.push_back(*child);
                    }
                    dependent_term.push_back(std::move(new_dependent));
                    dependent_term.relax(variable);
                }
            }
        }else if(_type == STAR){
            auto [new_independent, new_dependent] = _children[0]->distribute_variable(variable);
            if (new_independent._type != EMPTY) {
                independent_term = CommutativeRegex(STAR, "*");
                independent_term.push_back(std::move(new_independent));
            }
            if (new_dependent._type != EMPTY){
                dependent_term = CommutativeRegex(CONCAT, ".");
                dependent_term.push_back(*this);
                dependent_term.push_back(std::move(new_dependent));
                dependent_term.relax(variable);
            }
        }else if(_type == TERMINAL){
            independent_term = *this;
        }else if(_type == NONTERMINAL){
            dependent_term = *this;
        }
        return {independent_term, dependent_term};
    }

    void decrease_power(const GrammarUnit& variable){
        if(_type == ALTERNATIVE){
            for(auto& child: _children)
                child->decrease_power(variable);
            __remove_empty();
        }else if(_type == CONCAT){
            if(__terms_power(variable) != 0) {
                _children.back()->decrease_power(variable);
                __remove_empty();
            }
        }else if(_type == NONTERMINAL){
            if (_data == variable.data()){
                if(_power == 1){
                    _type = EMPTY;
                    _data = "";
                }else{
                    _power--;
                }
            }
        }
    }

    void relax(const GrammarUnit& variable){
        bool is_relaxed = true;
        while(is_relaxed) {
            __release_brackets();
            __reorder(variable);
            __commute_powers(variable);
            __distribute_powers(variable);
            is_relaxed = __remove_empty();
        }
    }

    template<typename F>
    bool contains_if(F f) const noexcept{
        bool result = f(*this);
        for(std::uint32_t idx = 0; idx < _children.size() && !result; ++idx)
            result |= _children[idx]->contains_if(f);
        return result;
    }

    bool contains(const std::string& data) const noexcept{
        bool result = _data == data;
        for(std::uint32_t idx = 0; idx < _children.size() && !result; ++idx)
            result |= _children[idx]->contains(data);
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

    bool operator==(const CommutativeRegex& regex) const noexcept{
        bool result = (_type == regex._type) && (_data == regex._data) && (_power == regex._power);
        result &= _children.size() == regex._children.size();
        for( std::uint32_t idx = 0; idx < _children.size() && result; ++idx )
            if ( _children[idx] != nullptr && regex._children[idx] != nullptr )
                result &= *_children[idx] == *regex._children[idx];
        return result;
    }

    bool operator!=(const CommutativeRegex& regex) const noexcept{ return !(*this == regex); }
    friend std::string to_string(const CommutativeRegex& regex) noexcept{
        std::ostringstream os;
        os << regex;
        return os.str();
    }

    friend std::ostream& operator<<(std::ostream& out, const CommutativeRegex& regex){
        switch (regex._type) {
            case EMPTY:{
                out << "eps" << (regex._power > 1 ? "^" + std::to_string(regex._power) : "");
                break;
            }
            case NONTERMINAL: {
                out << regex._data << (regex._power > 1 ? "^" + std::to_string(regex._power) : "");
                break;
            }
            case TERMINAL: {
                out << regex._data << (regex._power > 1 ? "^" + std::to_string(regex._power) : "");
                break;
            }
            case ALTERNATIVE:{
                if (regex._power != 1) out << "(";
                for (std::uint32_t idx = 0; idx < regex._children.size() - 1; idx++)
                    out << *regex._children[idx] << "+";
                out << *regex._children.back();
                if (regex._power != 1) out << ")^" << std::to_string(regex._power);
                break;
            }
            case CONCAT: {
                if (regex._power != 1) out << "(";
                for (std::uint32_t idx = 0; idx < regex._children.size() - 1; idx++)
                    if(regex._children[idx]->_type == ALTERNATIVE && regex._children[idx]->_power == 1)
                        out << "(" << *regex._children[idx] << ")";
                    else
                        out << *regex._children[idx] << "";
                out << *regex._children.back();
                if (regex._power != 1) out << ")^" << std::to_string(regex._power);
                break;
            }
            case STAR: {
                if ((regex._children[0]->_type == TERMINAL || regex._children[0]->_type == NONTERMINAL) && regex._children[0]->power() == 1)
                    out << *regex._children[0] << "*";
                else
                    out << '(' << *regex._children[0] << ")*";
                break;
            }
        }
        return out;
    }

    ~CommutativeRegex(){
        for( auto& ptr: _children ) delete ptr;
    }

private:
    void __copy_assign(const CommutativeRegex& regex){
        _type = regex._type;
        _data = regex._data;
        _power = regex._power;
        for(const auto node_ptr: regex._children )
            if(node_ptr){
                _children.push_back(new CommutativeRegex());
                _children.back()->__copy_assign(*node_ptr);
            }
    }

    CommutativeRegex __term_without_variable(const GrammarUnit& variable){
        CommutativeRegex result;
        if(node_type() == CONCAT) {
            result.node_type() = CONCAT;
            result.data() = ".";
            for (const auto child: _children)
                if(child->node_type() != NONTERMINAL || child->data() != variable.data())
                    result.push_back(*child);
        }
        if (result._children.size() == 1)
            return *result._children[0];
        else if (result._children.size() == 0)
            return CommutativeRegex();
        else
            return result;
    }

    std::uint32_t __terms_power(const GrammarUnit& variable) const noexcept{
        std::uint32_t result = 0;
        if(node_type() == CONCAT) {
            for (const auto child: _children)
                result += (child->node_type() == NONTERMINAL && child->data() == variable.data() ? child->power() : 0);
        }else if(node_type() == NONTERMINAL && data() == variable.data()){
            result = power();
        }
        return result;
    }

    static bool __reorder_less(const CommutativeRegex& lhs, const CommutativeRegex& rhs) noexcept{ return lhs.data() < rhs.data(); }
    static bool __compactify_eq(const CommutativeRegex& lhs, const CommutativeRegex& rhs) noexcept{
        bool result = (lhs._type == rhs._type) && (lhs._data == rhs._data);
        result &= lhs._children.size() == rhs._children.size();
        for( std::uint32_t idx = 0; idx < lhs._children.size() && result; ++idx )
            if ( lhs._children[idx] != nullptr && rhs._children[idx] != nullptr )
                result &= *lhs._children[idx] == *rhs._children[idx];
        return result;
    }

    void __release_brackets(){
        if(_type == CONCAT || _type == ALTERNATIVE){
            auto type = _type == CONCAT ? CONCAT: ALTERNATIVE;
            auto it = std::find_if(_children.begin(),_children.end(),
                                   [&type](const CommutativeRegex* ptr){ return ptr->_type == type; });
            while( it != _children.end()){
                const auto __idx = std::distance(_children.begin(), it);
                for(std::uint32_t idx = 0; idx < _children[__idx]->_children.size(); ++idx)
                    _children.push_back(_children[__idx]->_children[idx]);
                _children[__idx]->_children.clear();
                _children.erase(_children.begin() + __idx);
                it = std::find_if(_children.begin(),_children.end(),
                                  [&type](const CommutativeRegex* ptr){ return ptr->_type == type; });
            }
        }
        for (auto& node_ptr: _children)
            node_ptr->__release_brackets();
    }
    // Allowed due to commutative rule for commutative alphabet
    void __reorder(const GrammarUnit& variable ){
        if(_type == CONCAT){
            auto metric = [&variable](const CommutativeRegex& regex)-> std::uint8_t {
                if(regex._type == NONTERMINAL && regex._data == variable.data())
                    return 3;
                if(regex._type == TERMINAL)
                    return 2;
                if(regex._type == NONTERMINAL)
                    return 1;
                return 0;
            };
            auto cmp = [&metric](CommutativeRegex* lhs, CommutativeRegex* rhs){
                const std::uint8_t lhs_metric = metric(*lhs), rhs_metric = metric(*rhs);
                if(lhs_metric == rhs_metric)
                    return CommutativeRegex::__reorder_less(*lhs, *rhs);
                else
                    return lhs_metric < rhs_metric;
            };
            std::sort(_children.begin(), _children.end(), cmp);
        }
        for (auto& node_ptr: _children)
            node_ptr->__reorder(variable);
    }

    void __commute_powers(const GrammarUnit& variable){
        if(_type == CONCAT){
            // Allowed due to commutative rule for commutative alphabet
            std::vector<CommutativeRegex*> compact;
            std::vector<bool> visited(_children.size(), false);
            for(std::uint32_t i = 0; i < _children.size(); i++){
                if(!visited[i]) {
                    visited[i] = true;
                    std::uint32_t initial_power = _children[i]->_power;
                    std::uint32_t power = initial_power;
                    for (std::uint32_t j = i + 1; j < _children.size(); j++)
                        if (!visited[j] && __compactify_eq(*_children[i], *_children[j]) ) {
                            visited[j] = true;
                            power += _children[j]->_power;
                        }
                    if(power == initial_power){
                        compact.push_back(_children[i]);
                    }else if (power > initial_power){
                        compact.push_back(new CommutativeRegex());
                        *compact.back() = *_children[i];
                        compact.back()->power() = power;
                    }
                }
            }
            _children = compact;
        }
        for (auto& node_ptr: _children)
            node_ptr->__commute_powers(variable);
    }

    void __distribute_powers(const GrammarUnit& variable){
        if( _type == ALTERNATIVE){
            std::vector<CommutativeRegex*> compact;
            std::vector<bool> visited(_children.size(), false);
            for(std::uint32_t i = 0; i < _children.size(); i++){
                if(!visited[i]) {
                    visited[i] = true;
                    std::uint32_t power = _children[i]->__terms_power(variable);
                    CommutativeRegex new_term(ALTERNATIVE, "+");
                    new_term.push_back(_children[i]->__term_without_variable(variable));
                    for (std::uint32_t j = i + 1; j < _children.size(); j++)
                        if (!visited[j] && power == _children[j]->__terms_power(variable) && power != 0 ) {
                            visited[j] = true;
                            new_term.push_back(_children[j]->__term_without_variable(variable));
                        }
                    if(new_term._children.size() == 1){
                        compact.push_back(_children[i]);
                    }else if (new_term._children.size() > 1){
                        CommutativeRegex buffer(CONCAT, ".");
                        CommutativeRegex term_variable(NONTERMINAL, variable.data(), power);
                        buffer.push_back(std::move(new_term));
                        buffer.push_back(std::move(term_variable));
                        compact.push_back(new CommutativeRegex());
                        *compact.back() = std::move(buffer);
                    }
                }
            }
            _children = compact;
        }
        for (auto& node_ptr: _children)
            node_ptr->__distribute_powers(variable);
    }

    bool __remove_empty() noexcept{
        bool result = false;
        for(std::uint32_t idx = 0; idx < _children.size();){
            if(_children[idx]->node_type() == EMPTY){
                const auto tmp = _children[idx];
                _children.erase(_children.begin() + idx);
                delete tmp;
            }else{
                idx++;
            }
        }

        for(auto& child: _children)
            result |= child->__remove_empty();

        for(std::uint32_t idx = 0; idx < _children.size();){
            if(_children[idx]->_type != NONTERMINAL && _children[idx]->_type != TERMINAL && _children[idx]->_children.empty()){
                const auto tmp = _children[idx];
                _children.erase(_children.begin() + idx);
                delete tmp;
            }else{
                idx++;
            }
        }

        if(_children.size() == 1 && ( _type == CONCAT || _type == ALTERNATIVE)){
            const auto tmp = _children[0]->_children;
            _type = _children[0]->_type;
            _data = _children[0]->_data;
            _power = _children[0]->_power;
            _children[0]->_children.clear();
            delete _children[0];
            _children = tmp;
            result |= true;
        }
        return result;
    }
};