#pragma once
#include <list>
#include <set>
#include <unordered_map>
#include <gvc.h>

class Relation{
public:
    enum RelationCardinality : std::uint8_t {
        Optional = 0,
        Mandatory = 1,
        Many = 2
    };
    enum RelationDependence : std::uint8_t {
        None = 0,
        LHS_TO_RHS = 1,
        RHS_TO_LHS = 2
    };
private:
    std::string _lhs;
    std::list<std::string> _rhs;
    // lhs to rhs min/max cardinalities
    std::pair<RelationCardinality, RelationCardinality> _lhs_cardinality;
    /*
     * rhs to lhs min/max cardinalities
     * There's no need in cardinalities with "type-subtype" template, so just 1 pair
     */
    std::pair<RelationCardinality, RelationCardinality> _rhs_cardinality;
    RelationDependence _id_dependence{};
    bool _is_subtypes_inclusive{};
public:
    Relation() = default;

    inline const std::string& lhs() const noexcept{ return _lhs; }
    inline std::string& lhs() noexcept{ return _lhs; }
    inline const std::list<std::string>& rhs() const noexcept{ return _rhs; }
    inline std::list<std::string>& rhs() noexcept{ return _rhs; }
    inline const std::pair<RelationCardinality, RelationCardinality>& lhs_cardinality() const noexcept {return _lhs_cardinality;}
    inline std::pair<RelationCardinality, RelationCardinality>& lhs_cardinality() noexcept {return _lhs_cardinality;}
    void lhs_cardinality(const std::string& str) noexcept {
        _lhs_cardinality.first = str[0] == '0' ? Optional : (str[0] == '1' ? Mandatory : Many);
        _lhs_cardinality.second = str[2] == '0' ? Optional : (str[2] == '1' ? Mandatory : Many);
    }
    inline const std::pair<RelationCardinality, RelationCardinality>& rhs_cardinality() const noexcept {return _rhs_cardinality;}
    inline std::pair<RelationCardinality, RelationCardinality>& rhs_cardinality() noexcept {return _rhs_cardinality;}
    void rhs_cardinality(const std::string& str) noexcept {
        _rhs_cardinality.first = str[0] == '0' ? Optional : (str[0] == '1' ? Mandatory : Many);
        _rhs_cardinality.second = str[2] == '0' ? Optional : (str[2] == '1' ? Mandatory : Many);
    }
    inline const RelationDependence& id_dependence() const noexcept{ return _id_dependence; }
    inline RelationDependence& id_dependence() noexcept{ return _id_dependence; }
    inline const bool& is_subtypes_inclusive() const noexcept{ return _is_subtypes_inclusive; }
    inline bool& is_subtypes_inclusive() noexcept{ return _is_subtypes_inclusive; }
    bool operator==(const Relation& rhs) noexcept{
        std::set<std::string> lhs_dest(_rhs.begin(), _rhs.end()), rhs_dest(rhs._rhs.begin(),rhs._rhs.end());
        return _lhs == rhs._lhs && lhs_dest == rhs_dest && _lhs_cardinality == rhs._lhs_cardinality &&
               _rhs_cardinality == rhs._rhs_cardinality;
    }
    static std::string  cardinality_to_format(RelationCardinality cardinality){
        if (cardinality == Optional){
            return "odot";
        }else if( cardinality == Mandatory){
            return "tee";
        }else{
            return "oinv";
        }
    }

    static std::string cardinality_format(std::pair<RelationCardinality, RelationCardinality> cardinality){
        std::string result;
        result+= cardinality_to_format(cardinality.second);
        result+= cardinality_to_format(cardinality.first);
        return result;
    }

    Agedge_t* construct_edge(Agraph_t* graph, std::unordered_map<std::string, Agnode_t*> verticies) const noexcept{
        Agedge_t* result;
        if (_rhs.size() > 1){
            Agnode_t* intermidiate_node = agnode(graph, (_lhs + *_rhs.begin()).data(), 1);
            agsafeset(intermidiate_node, "shape", "circle", "");
            std::string tmp = _is_subtypes_inclusive?"X":"";
            agsafeset(intermidiate_node, "label", tmp.data(), "");
            Agedge_t* intermidiate_edge = agedge(graph, verticies[_lhs], intermidiate_node, (_lhs + *_rhs.begin()).data(), 1);
            for(const auto& entity: _rhs){
                Agedge_t* buffer =  agedge(graph, intermidiate_node, verticies[entity], (_lhs + entity +"1").data(), 1);
            }
        }else{
            result = agedge(graph, verticies[_lhs], verticies[*_rhs.begin()], "abc", 1);
            agsafeset(result, "dir", "both", "");
            if(id_dependence() == None){
                agsafeset(result, "style", "dashed", "");
            }
            std::string lhs_format = cardinality_format(_lhs_cardinality);
            std::string rhs_format = cardinality_format(_rhs_cardinality);
            agsafeset(result, "arrowhead", lhs_format.data(), "");
            agsafeset(result, "arrowtail", rhs_format.data(), "");
        }


        return result;
    }
};