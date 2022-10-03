#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <common/fxtbits-set.h>
#include <common/string-extension.h>
#include <common/stream-extension.h>
#include <common/perf.h>



class Grammar{
    char_bitset_type _constructors;
    char_bitset_type _variables;
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
    char_bitset_type _variables;
public:
    Term(const char name = 0): _name(name){}

    void identify(const Grammar& grammar){
        if(_children.empty()){
            if(grammar.is_variable(_name)) _variables.insert(_name);
        }else{
            for(const auto tptr: _children) {
                tptr->identify(grammar);
                _variables |= tptr->_variables;
            }
        }
    }

    inline bool is_variable() const noexcept{ return _children.empty() && _variables.contains(_name);}
    inline char name() const noexcept{ return _name; }
    inline std::uint64_t size() const noexcept{ return _children.size(); }
    inline const std::vector<Term*>& children() const noexcept{ return _children; }
    inline std::vector<Term*>& children() noexcept{ return _children; }
    inline const char_bitset_type& variables() const noexcept{ return _variables; }

    friend std::istream& operator>>(std::istream& in, Term& term) noexcept{
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
        for(const auto& tptr: _children) delete tptr;
    }
};

class MultiEquation{
    /* There's need more explanation.
    * We don't need to rewrite terms in U set due to unification algorithm#3, so we don't need to allocate new terms
    * Since then, term1 == term2 if &term1 == &term2.
    */
    char_bitset_type _lhs;
    std::set<const Term*> _rhs;
    char_bitset_type _rhs_variables;
public:

    MultiEquation() = default;
    MultiEquation(const std::initializer_list<std::uint64_t>& variables){
        for(const auto variable: variables) _lhs.insert(variable);
    }
    MultiEquation(const std::initializer_list<std::uint64_t>& variables, const std::initializer_list<const Term*>& terms){
        for(const auto variable: variables) _lhs.insert(variable);
        _rhs.insert(terms.begin(), terms.end());
        for(const auto tptr: _rhs) _rhs_variables |= tptr->variables();
    }
    MultiEquation(const char_bitset_type & variables):_lhs(variables){}
    MultiEquation(const char_bitset_type & variables, const std::initializer_list<const Term*>& terms): _lhs(variables)
    {
        _rhs.insert(terms.begin(), terms.end());
        for(const auto tptr: _rhs) _rhs_variables |= tptr->variables();
    }

    [[nodiscard]] char_bitset_type intersect(const MultiEquation& multiEquation) const noexcept{
        return _lhs & multiEquation._lhs;
    }
    void merge(const MultiEquation& multiEquation) noexcept{
        _lhs |= multiEquation._lhs;
        _rhs_variables |= multiEquation._rhs_variables;
        _rhs.insert(multiEquation._rhs.begin(), multiEquation._rhs.end());
    }
    void merge(MultiEquation&& multiEquation) noexcept{
        _lhs |= multiEquation._lhs;
        _rhs_variables |= multiEquation._rhs_variables;
        _rhs.merge(std::move(multiEquation._rhs));
    }
    void insert_lhs(const char variable) noexcept{ _lhs.insert(variable); }
    void insert_rhs(const Term* term) noexcept{ _rhs.insert(term); _rhs_variables |= term->variables(); }
    inline const char_bitset_type lhs() const noexcept{ return _lhs; }
    inline const std::set<const Term*>& rhs() const noexcept{ return _rhs; }
    inline const char_bitset_type rhs_variables() const noexcept{ return _rhs_variables; }

    friend std::ostream& operator<<(std::ostream& out, const MultiEquation& multeq){
        out << '{';
        bool f{};
        multeq._lhs.for_each_bit([&out, &f](const std::uint64_t value){
            out << (f ? ", " : (f = 1, ""));
            if (value == 0) out << "x0"; else out << (char)value;
        });
        out << "} = [";
        bool o{};
        for (const auto& e : multeq._rhs)
            out << (o ? ", " : (o = 1, "")) << *e;
        return out << "]";
    }
    void free(){ for(const auto tptr: _rhs){ delete tptr; } }
};

class MultiEquationSet{
    typedef typename std::vector<MultiEquation>::const_iterator const_iterator;
    std::vector<MultiEquation> _data;
public:
    MultiEquationSet() = default;

    [[nodiscard]] inline bool empty() const noexcept{ return _data.empty(); }
    void insert(MultiEquation&& value) noexcept{
        MultiEquation res(value);
        for(auto it = _data.cbegin(); it != _data.end();){
            if(!it->intersect(value).empty()){
                res.merge(*std::make_move_iterator(it));
                _data.erase(it);
            }else{
                it++;
            }
        }
        _data.push_back(value);
    }
    void insert(const MultiEquation& value) noexcept{
        MultiEquation res(value);
        for(auto it = _data.cbegin(); it != _data.end();){
            if(!it->intersect(value).empty()){
                res.merge(*it);
                _data.erase(it);
            }else{
                it++;
            }
        }
        _data.push_back(res);
    }
    void erase(const_iterator it){ _data.erase(it); }
    void merge(const MultiEquationSet& rhs){ for(const auto& multeq: rhs._data) insert(multeq); }
    void merge( MultiEquationSet&& rhs){ for(auto&& multeq: rhs._data) insert(multeq);}

    const_iterator unique() const noexcept{
        for(auto iter = _data.cbegin(); iter != _data.cend(); iter++){
            if(is_unique(iter)) return iter;
        }
        return _data.cend();
    }
    inline const_iterator cend() const noexcept{ return _data.cend(); }
    void free(){
        for(auto& multeq: _data){ multeq.free(); }
    }
    friend std::ostream& operator<<(std::ostream& out, const MultiEquationSet& multiEquationSet) noexcept{
        for(const auto& multeq: multiEquationSet._data){out << multeq << '\n';}
        return out;
    }
private:
    bool is_unique(const_iterator it) const noexcept{
        bool res = true;
        for(auto iter = _data.cbegin(); iter != _data.cend() && res; iter++){
            if(iter == it) continue;
            res &= (iter->rhs_variables() & it->lhs()).empty();
        }
        return res;
    }
};

std::pair<Term*, MultiEquationSet> dec(const std::set<const Term*>& terms){
    const Term* variable_in_rhs_terms = nullptr;
    bool __root_name_matches = true;
    const char __root_name = (*terms.begin())->name();
    for(const auto tptr: terms){
        __root_name_matches &= tptr->name() == __root_name;
        if(tptr->is_variable()){variable_in_rhs_terms = tptr;}
    }
    if(variable_in_rhs_terms){
        // make-multeq
        MultiEquationSet s;
        MultiEquation multeq;
        for(const auto tptr: terms){
            if(tptr->is_variable()){
                multeq.insert_lhs(tptr->name());
            } else{
                multeq.insert_rhs(tptr);
            }
        }
        s.insert(std::move(multeq));
        return {new Term(variable_in_rhs_terms->name()),std::move(s)};
    }else if(__root_name_matches){
        Term* common_term = new Term(__root_name);
        MultiEquationSet frontier;
        std::set<const Term*> buffer;
        for(std::uint64_t idx = 0; idx < (*terms.begin())->size(); idx++){
            for(const auto tptr: terms) buffer.insert(tptr->children()[idx]);
            const auto [__common_term, __frontier] = dec(buffer);
            if (__common_term == nullptr) {
                delete common_term;
                return {nullptr, std::move(frontier)};
            }
            common_term->children().push_back(__common_term);
            frontier.merge(__frontier);
            buffer.clear();
        }
        return {common_term, std::move(frontier)};
    }else{
        return {nullptr, MultiEquationSet()};
    }
}

enum UnificationResult{
        OK = 0,
        TERMS_UNDEFINED,
        UNIQUE_MULTIEQUATION_NOT_FOUND,
        COMMON_TERM_NOT_FOUND
};

UnificationResult unify(const Term& lhs, const Term& rhs, MultiEquationSet& result){
    PERF("UNIFY")
    MultiEquationSet initial_set;
    if(lhs.variables().empty() || rhs.variables().empty()){
        return UnificationResult::TERMS_UNDEFINED;
    }
    lhs.variables().for_each_bit([&initial_set](const std::uint64_t value){
        initial_set.insert(MultiEquation({value}));
    });
    rhs.variables().for_each_bit([&initial_set](const std::uint64_t value){
        initial_set.insert(MultiEquation({value}));
    });
    initial_set.insert(MultiEquation({0},{&lhs, &rhs}));
    while( !initial_set.empty() ){
        const auto unique_equation = initial_set.unique();
        if (unique_equation == initial_set.cend()){
            return UnificationResult::UNIQUE_MULTIEQUATION_NOT_FOUND;
        }
        if(unique_equation->rhs().empty()){
            result.insert(MultiEquation(unique_equation->lhs(),{}));
            initial_set.erase(unique_equation);
        }else{
            const auto [common_term, frontier] = dec(unique_equation->rhs());
            if (common_term == nullptr) return UnificationResult::COMMON_TERM_NOT_FOUND;
            result.insert(MultiEquation(unique_equation->lhs(),{common_term}));
            initial_set.erase(unique_equation);
            initial_set.merge(frontier);
        }
    }
    return UnificationResult::OK;
}

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
    // std::cout << first_term << '\n' << second_term << '\n';
    first_term.identify(grammar); second_term.identify(grammar);
    MultiEquationSet multiEquationSet;
    const auto res = unify(first_term,second_term, multiEquationSet);
    if(res == UnificationResult::OK){
        std::cout << multiEquationSet;
    }else{
        std::cout << "not unified\n";
    }
    multiEquationSet.free();
    return 0;
}