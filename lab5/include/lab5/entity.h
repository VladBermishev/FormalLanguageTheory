#pragma once
#include <lab5/attribute.h>
#include <list>
#include <gvc.h>

class Entity{
    std::string _name;
    std::list<Attribute> _identifiers;
    std::list<Attribute> _attributes;
    bool _is_weak{};
public:
    Entity() = default;
    explicit Entity(const std::string& name): _name(name) {}

    void add_attribute(const Attribute& attribute){ _attributes.push_back(attribute); }
    void add_attribute(Attribute&& attribute){ _attributes.push_back(std::move(attribute)); }
    void add_identifier(const Attribute& attribute){ _identifiers.push_back(attribute); }
    void add_identifier(Attribute&& attribute){ _identifiers.push_back(std::move(attribute)); }

    inline const std::string& name() const noexcept{ return _name; }
    inline const std::list<Attribute>& attributes() const noexcept{return _attributes;}
    inline const std::list<Attribute>& identifiers() const noexcept{return _identifiers;}
    inline bool is_weak() const noexcept { return _is_weak; }
    inline bool& is_weak() noexcept { return _is_weak; }

    inline bool operator==(const Entity& rhs) const noexcept{ return _name == rhs._name;}
    Agnode_t* construct_node(Agraph_t* graph) const noexcept {
        Agnode_t* result;
        /* ... */
        return result;
    }
};