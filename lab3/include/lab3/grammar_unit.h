#include <utility>

#pragma once

class GrammarUnit{
public:
    enum UnitType{
        NonTerminal = 0,
        Terminal,
    };
private:
    UnitType _type;
    std::string _data;
public:
    GrammarUnit() = default;
    GrammarUnit(const UnitType& type, std::string data): _type(type), _data(std::move(data)){}

    inline const UnitType type() const noexcept{ return _type; }
    inline const std::string& data() const noexcept{ return _data; }

    inline bool operator==(const GrammarUnit& rhs) const noexcept{
        return _type == rhs._type && _data == rhs._data;
    }
};
