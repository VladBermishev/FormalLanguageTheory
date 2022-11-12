#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "cxxabi.h"

template<class T>
std::string get_type_name(T &&obj) { return abi::__cxa_demangle(typeid(obj).name(), NULL, NULL, NULL); }

#define TYPE_TO_STRING(type) std::string( #type " = ")+get_type_name(type())
#define __S1(x) #x
#define __S2(x) __S1(x)
#define LOCATION __FILE__ " : " __S2(__LINE__)

class ParserOption {
private:
    const std::string _desc;
    const std::string _default_value;
public:
    template<std::uint32_t N>
    ParserOption(const char (& option_description)[N]) : _desc(option_description), _default_value("") {}

    template<std::uint32_t N, std::uint32_t M>
    ParserOption(const char (& option_description)[N], const char (& default_value)[M]) : _desc(option_description),
                                                                                          _default_value(
                                                                                                  default_value) {}

    inline const std::string& default_value() const noexcept { return _default_value; }

    friend std::ostream& operator<<(std::ostream& out, const ParserOption& opt) {
        if (opt._default_value.length() != 0) {
            return out << opt._desc << " [Default value = " << opt._default_value << "]";
        } else {
            return out << opt._desc;
        }
    }

};

class PositionalOptionsDescription {
private:
    const std::string _title;
    std::vector<ParserOption> _options;
public:

    explicit PositionalOptionsDescription(const std::string& title) : _title(title) {
        _options.reserve(10);
        add_options(ParserOption("Path to executable"));
    }

    explicit PositionalOptionsDescription(std::string&& title) : _title(title) {
        _options.reserve(10);
        add_options(ParserOption("Path to executable"));
    }

    template<typename... Args>
    void add_options(ParserOption&& option, Args&& ... args) {
        _options.push_back(option);
        add_options(std::forward<Args>(args)...);
    }


    inline std::uint32_t size() const noexcept { return _options.size(); }

    inline const ParserOption& operator[](const std::uint32_t index) const { return _options[index]; }

    friend std::ostream& operator<<(std::ostream& out, const PositionalOptionsDescription& opts) noexcept {
        out << opts._title << '\n';
        for (std::uint32_t idx = 1; idx < opts._options.size(); idx++) {
            out << std::to_string(idx) << ". " << opts._options[idx] << "\n";
        }
        return out;
    }

private:
    void add_options() {}
};

class CmdlinePositionalParser {
private:
    class parser_value {
    private:
        const std::string_view _value;
    public:
        explicit parser_value(const std::string_view value) noexcept: _value(value) {}

        template<typename T>
        T as() const {
            if constexpr(std::is_same_v<T, std::string>)
                return std::string(_value);
            else if constexpr(std::is_constructible_v<T, std::string>)
                return T(std::string(_value));
            else
                throw std::logic_error(LOCATION" - Unable to construct type " + TYPE_TO_STRING(T) + " from string\n");
        }
    };

    std::vector<parser_value> _values{};
    const PositionalOptionsDescription _desc;
public:
    CmdlinePositionalParser(const std::int32_t argc, const char* argv[], const PositionalOptionsDescription& desc)
            : _desc(desc) {
        _values.reserve(10);
        for (std::uint32_t idx = 0; idx < _desc.size(); idx++) {
            if (idx < argc)
                _values.emplace_back(std::string_view(argv[idx]));
            else if (const auto& default_value = _desc[idx].default_value(); default_value.length() != 0)
                _values.emplace_back(default_value);
            else {
                std::cerr << _desc << '\n';
                throw std::invalid_argument(LOCATION" Invalid number of obligatory parameters");
            }
        }
    }

    parser_value operator[](const std::uint32_t index) const {
        if (index >= 0 && index < _values.size())
            return _values[index];
        else {
            std::cerr << _desc << '\n';
            throw std::invalid_argument(LOCATION" Given index =" + std::to_string(index) + "is invalid\n");
        }
    }
};