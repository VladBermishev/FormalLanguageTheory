#pragma once
#include <common/fxtbits.h>
#include <string>

class Attribute{
    std::string _name;
    FxtBits _properties;
public:
    Attribute() = default;
    Attribute(const std::string& name, const bool is_alternate_key = false, const bool is_foreign_key = false,
              const bool is_primary_key = false){
        _name = name;
        if (is_alternate_key) _properties.set(0);
        if (is_foreign_key) _properties.set(1);
        if (is_primary_key) _properties.set(2);
    }
    inline bool is_alternate_key() const noexcept { return static_cast<bool>(_properties.get(0)); }
    inline bool is_foreign_key() const noexcept { return static_cast<bool>(_properties.get(1)); }
    inline bool is_primary_key() const noexcept { return static_cast<bool>(_properties.get(2)); }
    inline void set_alternate_key(const bool value) noexcept { if(value) _properties.set(0); else _properties.clear(0); }
    inline void set_foreign_key(const bool value) noexcept { if(value) _properties.set(1); else _properties.clear(1); }
    inline void set_primary_key(const bool value) noexcept { if(value) _properties.set(2); else _properties.clear(2); }
    inline const std::string& raw_name() const noexcept{ return _name; }
    std::string name() const noexcept{
        std::string result = _name;
        if(is_primary_key()) result += "(PK)";
        else if(is_alternate_key()) result += "(AK)";
        if(is_foreign_key()) result += "(FK)";
        return result;
    }
    inline bool operator<(const Attribute& attribute) const noexcept{
        return std::tie(_name, _properties) < std::tie(attribute._name, attribute._properties);
    }
    inline bool operator==(const Attribute& attribute) const noexcept{
        return std::tie(_name, _properties) == std::tie(attribute._name, attribute._properties);
    }
};