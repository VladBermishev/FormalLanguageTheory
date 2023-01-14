#pragma once
#include <common/fxtbits.h>
#include <string>

class Attribute{
    std::string _name;
    FxtBits _properties;
public:
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
    inline const std::string& name() const noexcept{ return _name; }
};