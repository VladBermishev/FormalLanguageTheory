#pragma once
#include <lab1/grammar.h>
#include <ostream>
#include <vector>

class Term {
    char _name;
    std::vector<Term*> _children;
    char_bitset_type _variables;
public:
    enum TermType{
        CONSTRUCTOR = 0,
        VARIABLE = 1
    };

    Term(const char name = 0, const TermType type = TermType::CONSTRUCTOR) : _name(name) {
        if (type) _variables.insert(_name);
    }

    void identify(const Grammar& grammar) {
        if (_children.empty()) {
            if (grammar.is_variable(_name)) _variables.insert(_name);
        } else {
            for (const auto tptr: _children) {
                tptr->identify(grammar);
                _variables |= tptr->_variables;
            }
        }
    }
    inline bool is_variable() const noexcept { return _children.empty() && _variables.contains(_name); }

    inline char name() const noexcept { return _name; }

    inline std::uint64_t size() const noexcept { return _children.size(); }

    inline const std::vector<Term*>& children() const noexcept { return _children; }

    inline std::vector<Term*>& children() noexcept { return _children; }

    inline const char_bitset_type& variables() const noexcept { return _variables; }

    inline void insert_child(Term* child) noexcept{
        if(child) {
            _children.push_back(child);
            _variables |= child->_variables;
        }
    }

    friend std::istream& operator>>(std::istream& in, Term& term) noexcept {
        in >> term._name;
        if (in.peek() == '(') {
            int __c = in.get();
            while (__c != ')') {
                term._children.push_back(new Term());
                in >> *term._children.back();
                __c = in.get();
            }
        }
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, const Term& term) noexcept {
        out << term._name;
        if (!term._children.empty()) {
            out << '(';
            for (std::uint32_t idx = 0; idx < term._children.size() - 1; idx++) {
                out << *term._children[idx] << ',';
            }
            out << *term._children.back() << ')';
        }
        return out;
    }
    ~Term() {
        for (const auto& tptr: _children) delete tptr;
    }
};

struct TermCompare{
    bool operator()(const Term* const lhs, const Term* const rhs) const noexcept{ return __compare(lhs, rhs) < 0; }
private:
    static std::int32_t __compare(const Term* const lhs, const Term* const rhs) noexcept{
        if (lhs == nullptr || rhs == nullptr) return 0;
        if (lhs == rhs) return 0;
        if (lhs->name() == rhs->name()) {
            std::int32_t __res = 0;
            const std::uint64_t __sz = std::min(lhs->children().size(), rhs->children().size());
            for (std::uint64_t idx = 0; idx < __sz && !__res; idx++)
                __res = __compare(lhs->children()[idx], rhs->children()[idx]);
            return __res;
        } else {
            return lhs->name() < rhs->name() ? -1 : 1;
        }
    }
};