#pragma once
#include <common/fxtbits-set.h>

class Rule{
    char_bitset_type _variables;
    Regex _lhs;
    Regex _rhs;
public:
    Rule() = default;
    Rule(Regex&& lhs, Regex&& rhs) noexcept : _lhs(std::move(lhs)), _rhs(std::move(rhs)){}

    [[nodiscard]] inline const Regex& lhs() const noexcept{ return _lhs; }
    [[nodiscard]] inline const Regex& rhs() const noexcept{ return _rhs; }
    [[nodiscard]] inline const char_bitset_type& variables() const noexcept{ return _variables; }
    [[nodiscard]] inline char_bitset_type& variables() noexcept{ return _variables; }

};