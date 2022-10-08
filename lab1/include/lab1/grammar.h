#pragma once
#include <common/fxtbits-set.h>
#include <common/string-extension.h>
#include <common/stream-extension.h>

class Grammar {
    char_bitset_type _constructors;
    char_bitset_type _variables;
public:
    Grammar() = default;

    [[nodiscard]] inline bool is_constructor(const char c) const noexcept { return _constructors.contains(c); }

    [[nodiscard]] inline bool is_variable(const char c) const noexcept { return _variables.contains(c); }

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