#pragma once
#include <set>
#include <lab1/term.h>
#include <common/fxtbits-set.h>



class MultiEquation {
    char_bitset_type _lhs;
    std::set<const Term*, TermCompare> _rhs;
    char_bitset_type _rhs_variables;
public:
    typedef std::set<const Term*, TermCompare> term_set_type;

    MultiEquation() = default;

    MultiEquation(const std::initializer_list<std::uint64_t>& variables){
        for (const auto variable: variables) _lhs.insert(variable);
    }

    MultiEquation(const std::initializer_list<std::uint64_t>& variables,
                  const std::initializer_list<const Term*>& terms){
        for (const auto variable: variables) _lhs.insert(variable);
        _rhs.insert(terms.begin(), terms.end());
        for (const auto tptr: _rhs) _rhs_variables |= tptr->variables();
    }

    MultiEquation(const char_bitset_type& variables) : _lhs(variables){}

    MultiEquation(const char_bitset_type& variables, const std::initializer_list<const Term*>& terms) : _lhs(variables)
    {
        _rhs.insert(terms.begin(), terms.end());
        for (const auto tptr: _rhs) _rhs_variables |= tptr->variables();
    }

    [[nodiscard]] char_bitset_type intersect(const MultiEquation& multiEquation) const noexcept {
        return _lhs & multiEquation._lhs;
    }

    void merge(const MultiEquation& multiEquation) noexcept {
        _lhs |= multiEquation._lhs;
        _rhs_variables |= multiEquation._rhs_variables;
        _rhs.insert(multiEquation._rhs.begin(), multiEquation._rhs.end());
    }

    void merge(MultiEquation&& multiEquation) noexcept {
        _lhs |= multiEquation._lhs;
        _rhs_variables |= multiEquation._rhs_variables;
        _rhs.merge(std::move(multiEquation._rhs));
    }

    void insert_lhs(const char variable) noexcept { _lhs.insert(variable); }

    void insert_rhs(const Term* term) noexcept {
        _rhs.insert(term);
        _rhs_variables |= term->variables();
    }

    inline const char_bitset_type lhs() const noexcept { return _lhs; }
    inline const term_set_type& rhs() const noexcept { return _rhs; }
    inline const char_bitset_type rhs_variables() const noexcept { return _rhs_variables; }

    friend std::ostream& operator<<(std::ostream& out, const MultiEquation& multeq) {
        out << '{';
        bool f{};
        multeq._lhs.for_each_bit([&out, &f](const std::uint64_t value) {
            out << (f ? ", " : (f = 1, ""));
            if (value == 0) out << "x0"; else out << (char) value;
        });
        out << "} = {";
        bool o{};
        for (const auto& e: multeq._rhs)
            out << (o ? ", " : (o = 1, "")) << *e;
        return out << "}";
    }

    void free() { for (const auto tptr: _rhs) { delete tptr; }}
};