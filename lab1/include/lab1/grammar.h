#pragma once
#include "common/fxtbits-set.h"
#include "common/string-extension.h"
#include "common/stream-extension.h"

class Grammar {
    char_bitset_type _constructors;
    char_bitset_type _variables;
    char_bitset_type _constants;
public:
    Grammar() = default;

    [[nodiscard]] inline const char_bitset_type& constructors() const noexcept{ return _constructors; }
    [[nodiscard]] inline const char_bitset_type& variables() const noexcept{ return _variables; }
    [[nodiscard]] inline const char_bitset_type& constants() const noexcept{ return _constants; }

    [[nodiscard]] inline bool is_constructor(const char c) const noexcept { return _constructors.contains(c); }
    inline void insert_constructor(const char c) noexcept { _constructors.insert(c); }
    [[nodiscard]] inline bool is_variable(const char c) const noexcept { return _variables.contains(c); }
    inline void insert_variable(const char c) noexcept { _variables.insert(c); }
    [[nodiscard]] inline bool is_constant(const char c) const noexcept { return _constants.contains(c); }
    inline void insert_constant(const char c) noexcept { _constants.insert(c); }

    std::istream& parse_constructors(std::istream& in) {
        std::string line;
        std::getline(in, line);
        for (const auto var: split(line.c_str(), line.size(), ',')) {
            _constructors.insert(*var.data());
        }
        return in;
    }

    std::istream& parse_variables(std::istream& in) {
        std::string line;
        std::getline(in, line);
        for (const auto var: split(line.c_str(), line.size(), ',')) {
            _variables.insert(*var.data());
        }
        return in;
    }
};