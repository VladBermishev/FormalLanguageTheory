#pragma once
#include <lab5/entity.h>
#include <lab5/relation.h>
#include <common/stream-extension.h>
#include <common/string-extension.h>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <filesystem>
#include <gvc.h>

class EntityRelationshipDiagram{
    std::list<Entity> _entities;
    std::list<Relation> _relations;
    struct GrammarAttribute{
        std::string entity_name;
        std::string entity_field;
        std::string operation;
        std::string rhs;
        friend istream_extension& operator>>(istream_extension& in, GrammarAttribute& attribute) noexcept{
            in.lstrip(" ");
            std::getline(in,attribute.entity_name,'.');
            while(in.peek() != std::char_traits<char>::eof() && in.peek() != '\n' && is_alpha(in.peek())) attribute.entity_field.push_back(in.get());
            in.lstrip(" ");
            while(in.peek() != std::char_traits<char>::eof() && in.peek() != '\n' && !is_alpha(in.peek()) && in.peek() != '('){
                if(in.peek() == ' '){ in.get(); continue; }
                attribute.operation.push_back(in.get());
            }
            if(in.peek() == '(') {
                while (in.peek() != std::char_traits<char>::eof() && in.peek() != '\n' && in.peek() != ')') attribute.rhs.push_back(in.get());
                attribute.rhs.push_back(in.get());
                if (in.peek() != std::char_traits<char>::eof() && in.peek() != '\n') in.get();
            }else{
                while (in.peek() != std::char_traits<char>::eof() && in.peek() != '\n' && in.peek() != ',') attribute.rhs.push_back(in.get());
                if (in.peek() != std::char_traits<char>::eof() && in.peek() != '\n') in.get();
            }
            attribute.rhs.erase(std::remove_if(attribute.rhs.begin(), attribute.rhs.end(),[](const char ch){ return ch == ' ';}), attribute.rhs.end());
            return in;
        }
    };
public:
    EntityRelationshipDiagram() = default;

    bool contains_entity(const Entity& entity) const noexcept{
        const auto it = std::find(_entities.begin(), _entities.end(),entity);
        return it != _entities.end();
    }
    bool contains_relation(const Relation& relation){
        const auto it = std::find(_relations.begin(), _relations.end(), relation);
        return it != _relations.end();
    }
    void add_entity(const Entity& entity) {
        if(contains_entity(entity))
            throw std::logic_error(string_format("Found unexpected %s entity redefinition", entity.name().c_str()));
        else
            _entities.push_back(entity);
    }
    void add_entity(Entity&& entity) {
        if(contains_entity(entity))
            throw std::logic_error(string_format("Found unexpected %s entity redefinition", entity.name().c_str()));
        else
            _entities.push_back(std::move(entity));
    }
    void add_relation(const Relation& relation) {
        if(contains_relation(relation))
            throw std::logic_error(string_format("Found unexpected %s -> %s relation redefinition", relation.lhs().c_str(),
                                                 relation.rhs().size() == 1 ? relation.rhs().begin()->c_str() : "..."));
        if(relation.rhs().size() == 1 && !contains_entity(Entity(relation.rhs().front())))
            throw std::logic_error(string_format("Found undefined %s entity in %s -> %s relation", relation.lhs().c_str(),
                                                 relation.lhs().c_str(), relation.rhs().begin()->c_str()));
        if(relation.rhs().size() == 1 && !contains_entity(Entity(relation.lhs())))
            throw std::logic_error(string_format("Found undefined %s entity in %s -> %s relation", relation.lhs().c_str(),
                                                 relation.lhs().c_str(), relation.rhs().begin()->c_str()));
        if (relation.id_dependence() == Relation::RHS_TO_LHS) {
            for (const auto& entity_name: relation.rhs()) {
                const auto it = std::find_if(_entities.begin(), _entities.end(), [&entity_name](const Entity& entity) {
                    return entity_name == entity.name();
                });
                it->is_weak() = true;
            }
        }else if (relation.id_dependence() == Relation::LHS_TO_RHS){
            const auto it = std::find_if(_entities.begin(), _entities.end(), [&relation](const Entity& entity) {
                return relation.lhs() == entity.name();
            });
            it->is_weak() = true;
        }
        _relations.push_back(relation);
    }
    void add_relation(Relation&& relation) {
        if(contains_relation(relation))
            throw std::logic_error(string_format("Found unexpected %s -> %s relation redefinition", relation.lhs().c_str(),
                                                 relation.rhs().size() == 1 ? relation.rhs().begin()->c_str() : "..."));
        _relations.push_back(std::move(relation));
    }
    EntityRelationshipDiagram& to_relational_model(){
        /* ... */
    }
    friend istream_extension& operator>>(istream_extension& in, EntityRelationshipDiagram& diagram){
        while(!in.eof()){
            const auto [lhs, rhs] = diagram.read_rule(in);
            const auto grammar_attributes = diagram.read_attributes(in);
            if(rhs.size() == 0){
                Entity entity(lhs);
                const auto attrs_cnt = std::count_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                     [](const GrammarAttribute& attribute){ return attribute.entity_field == "attr"; });
                const auto ident_cnt = std::count_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                     [](const GrammarAttribute& attribute){ return attribute.entity_field == "ident"; });
                if(attrs_cnt > 1) throw std::logic_error(string_format("Redefinition of %s: %d attributes declaration",lhs.c_str(),attrs_cnt));
                if(ident_cnt > 1) throw std::logic_error(string_format("Redefinition of %s: %d identifiers declaration",lhs.c_str(),ident_cnt));
                const auto attrs = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                             [](const GrammarAttribute& attribute){ return attribute.entity_field == "attr"; });
                const auto ident = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                [](const GrammarAttribute& attribute){ return attribute.entity_field == "ident"; });
                if(attrs != grammar_attributes.end() && attrs->entity_name != lhs)
                    throw std::logic_error(string_format("Unexpected definition of %s attributes", attrs->entity_name.c_str()));
                if(ident != grammar_attributes.end() && ident->entity_name != lhs)
                    throw std::logic_error(string_format("Unexpected definition of %s identifiers", ident->entity_name.c_str()));
                if(attrs != grammar_attributes.end())
                    for(const auto& attribute : split(attrs->rhs.c_str() + 1, attrs->rhs.length() - 2, ','))
                        entity.add_attribute(Attribute(std::string(attribute)));
                if(ident != grammar_attributes.end())
                    for(const auto& attribute : split(ident->rhs.c_str() + 1, ident->rhs.length() - 2, ','))
                        entity.add_identifier(Attribute(std::string(attribute)));
                diagram.add_entity(std::move(entity));
            }else if(rhs.size() == 1){
                Relation relation;
                relation.lhs() = lhs;
                relation.rhs() = rhs;
                const auto lhs_cardinalities = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                            [&lhs](const GrammarAttribute& attribute){
                                                                return attribute.entity_field == "cardinality" && attribute.entity_name == lhs;
                                                            });
                const auto rhs_cardinalities = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                            [&rhs](const GrammarAttribute& attribute){
                                                                return attribute.entity_field == "cardinality" && attribute.entity_name == *rhs.begin();
                                                            });
                if(lhs_cardinalities == grammar_attributes.end()) throw std::logic_error(string_format("%s -> %s relation cardinalities wasn't found", lhs.c_str(), rhs.begin()->c_str()));
                if(rhs_cardinalities == grammar_attributes.end()) throw std::logic_error(string_format("%s -> %s relation cardinalities wasn't found", rhs.begin()->c_str(), lhs.c_str()));
                relation.lhs_cardinality(lhs_cardinalities->rhs);
                relation.rhs_cardinality(rhs_cardinalities->rhs);
                const auto ident_dependance_cnt = std::count_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                           [&lhs, &rhs](const GrammarAttribute& attribute){
                                                               return attribute.operation == "::=" && attribute.entity_field == "ident";
                                                           });
                if (ident_dependance_cnt > 1)
                    throw std::logic_error(string_format("Unexpected %d identifier dependencies in %s -> %s relation", ident_dependance_cnt, lhs.c_str(), rhs.begin()->c_str()));
                const auto ident_dependance = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                           [&lhs, &rhs](const GrammarAttribute& attribute){
                                                               return attribute.operation == "::=" && attribute.entity_field == "ident" &&
                                                                       ( (attribute.entity_name == lhs && attribute.rhs == *rhs.begin() + ".ident") ||
                                                                         (attribute.entity_name == *rhs.begin() && attribute.rhs == lhs + ".ident"));
                                                           });
                if (ident_dependance != grammar_attributes.end())
                    relation.id_dependence() = ident_dependance->entity_name == lhs ? Relation::LHS_TO_RHS : Relation::RHS_TO_LHS;
                diagram.add_relation(std::move(relation));
            }else{
                Relation relation;
                std::list<Entity> subtypes;
                relation.lhs() = lhs;
                relation.rhs() = rhs;
                relation.id_dependence() = Relation::RHS_TO_LHS;
                for(const auto& entity: rhs){
                    Entity entity_to_push(entity);
                    entity_to_push.is_weak() = true;
                    const auto attrs_cnt = std::count_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                         [&entity](const GrammarAttribute& attribute){
                                                             return attribute.entity_field == "attr" && attribute.entity_name == entity;
                                                         });
                    if(attrs_cnt > 1) throw std::logic_error(string_format("Redefinition of %s: %d attributes declaration",lhs.c_str(),attrs_cnt));
                    const auto attrs = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                    [&entity](const GrammarAttribute& attribute){
                                                        return attribute.entity_field == "attr" && attribute.entity_name == entity;
                                                    });
                    if(attrs != grammar_attributes.end())
                        for(const auto& attribute : split(attrs->rhs.c_str() + 1, attrs->rhs.length() - 2, ','))
                            entity_to_push.add_attribute(Attribute(std::string(attribute)));
                    const auto it = std::find_if(diagram._entities.begin(), diagram._entities.end(),[&lhs](const Entity& e){
                        return e.name() == lhs;
                    });
                    if(it != diagram._entities.end()){
                        for(const auto& e: it->identifiers())
                            entity_to_push.add_identifier(e);
                    }else {
                        throw std::logic_error(string_format("Entity %s wasn't defined before %s -> ... relation", lhs.c_str(),
                                              lhs.c_str()));
                    }
                    subtypes.push_back(entity_to_push);
                }
                const auto template_type_cnt = std::count_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                        [](const GrammarAttribute& attribute){
                                                            return attribute.entity_field == "subtypes";
                                                        });
                if (template_type_cnt > 1)
                    throw std::logic_error(string_format("Unexpected %s template types definition in %s -> ... relation", template_type_cnt, lhs.c_str()));
                const auto template_type = std::find_if(grammar_attributes.begin(), grammar_attributes.end(),
                                                        [&lhs](const GrammarAttribute& attribute){
                                                            return attribute.entity_field == "subtypes" && attribute.entity_name == lhs;
                                                        });
                relation.is_subtypes_inclusive() = template_type->rhs == "inclusive";
                diagram.add_relation(std::move(relation));
                for(const auto& entity: subtypes)
                    diagram.add_entity(entity);
            }
        }
        return in;
    }

    int save_svg(const std::filesystem::path& path){
        GVC_t *gvc;
        gvc = gvContext();
        Agraph_t *g;
        g = agopen("g", Agdirected, nullptr);
        std::unordered_map<std::string, Agnode_t*> vertices;
        for(const auto& entity: _entities)
            vertices[entity.name()] = entity.construct_node(g);
        std::vector<Agedge_t*> edges;
        for(const auto& relation: _relations)
            edges.push_back(relation.construct_edge(g, vertices));
        gvLayout(gvc, g, "neato");
        gvRenderFilename(gvc, g, "svg", path.c_str());
        gvFreeLayout(gvc, g);
        agclose(g);
        return gvFreeContext(gvc);
    }
private:
    std::pair<std::string, std::list<std::string>> read_rule(istream_extension& in) noexcept {
        std::pair<std::string, std::list<std::string>> result;
        in >> result.first;
        in.ignore(stream_max_size,'>').lstrip(" ");
        std::string buffer;
        std::getline(in, buffer,';');
        auto it = std::remove_if(buffer.begin(),buffer.end(),[](const char ch){return ch == ' ';});
        buffer.erase(it, buffer.end());
        const auto entities = split(buffer.c_str(), buffer.length(), '|');
        for(const auto& entity: entities)
            result.second.emplace_back(entity);
        return result;
    }
    std::list<GrammarAttribute> read_attributes(istream_extension& in) noexcept{
        std::list<GrammarAttribute> result;
        in.lstrip(" ");
        while(in.peek() != '\n' && !in.eof()){
            GrammarAttribute attribute;
            in >> attribute;
            result.push_back(std::move(attribute));
        }
        return result;
    }
};