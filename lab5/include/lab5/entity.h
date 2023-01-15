#pragma once
#include <lab5/attribute.h>
#include <list>
#include <set>
#include <algorithm>
#include <string_view>
#include <string>
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
    inline std::list<Attribute>& attributes() noexcept{return _attributes;}
    inline const std::list<Attribute>& identifiers() const noexcept{return _identifiers;}
    inline std::list<Attribute>& identifiers() noexcept{return _identifiers;}
    inline bool is_weak() const noexcept { return _is_weak; }
    inline bool& is_weak() noexcept { return _is_weak; }

    inline bool operator==(const Entity& rhs) const noexcept{ return _name == rhs._name;}
    std::set<std::string> identifiers_intersection(const Entity& entity){
        std::set<Attribute> buffer, lhs(_identifiers.begin(), _identifiers.end());
        std::set<Attribute> rhs(entity._identifiers.begin(), entity._identifiers.end());
        std::set_intersection(lhs.begin(),lhs.end(),rhs.begin(),rhs.end(),std::inserter(buffer, buffer.begin()),
                              [](const Attribute& lhs, const Attribute& rhs){return lhs.raw_name() < rhs.raw_name(); });
        std::set<std::string> result;
        for(const auto& ident: buffer) result.insert(ident.raw_name());
        return result;
    }
    std::string generate_foreign_key_name(const std::string& prefix){
        std::string result;
        std::uint32_t cnt = std::count_if(_attributes.begin(),_attributes.end(),[&prefix](const Attribute& attribute){
            return attribute.name().find(prefix) == 0;
        });
        return cnt == 0 ? prefix : prefix + std::to_string(cnt);
    }

    Agnode_t* construct_node(Agraph_t* graph) noexcept {
        Agnode_t* result = agnode(graph, _name.data(), 1);
        std::string _label;
        agsafeset(result, "style", "filled", "");
        for(const auto& identifier: _identifiers)
            _label += identifier.name() + "\\n";
        _label += "|";
        for(const auto& attribute: _attributes)
            _label += attribute.name() + "\\n";
        _label = "{" + _label + "}";
        if (_is_weak){
            agsafeset(result, "shape", "Mrecord", "");
            agsafeset(result, "fillcolor", "lightblue", "");
        }else{
            agsafeset(result, "shape", "record", "");
            agsafeset(result, "fillcolor", "lightgreen", "");
        }
        agsafeset(result, "label", _label.data(), "");
        agsafeset(result, "xlabel", _name.data(), "");
        return result;
    }
};