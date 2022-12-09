#pragma once
#include <lab3/grammar_unit.h>
#include <lab3/regex.h>
#include <iostream>
#include <list>
#include <vector>

class Equation{
    GrammarUnit _lhs;
    Regex _rhs;
public:
    Equation() : _rhs(Regex::ALTERNATIVE, "+") {}

    Equation(GrammarUnit lhs) noexcept : _rhs(Regex::ALTERNATIVE, "+"){
        _lhs = std::move(lhs);
    }

    void append(const std::list<GrammarUnit>& rule_rhs){
        Regex term;
        if (rule_rhs.size() == 1){
            term.node_type() = (rule_rhs.front().type() == GrammarUnit::NonTerminal ? Regex::NONTERMINAL : Regex::TERMINAL);
            term.data() = rule_rhs.front().data();
        }else if( rule_rhs.size() > 1 ){
            term.node_type() = Regex::CONCAT;
            term.data() = ".";
            for(const auto& unit: rule_rhs){
                const auto type = (unit.type() == GrammarUnit::NonTerminal ? Regex::NONTERMINAL : Regex::TERMINAL);
                term.push_back(Regex(type, unit.data()));
            }
        }
        _rhs.push_back(std::move(term));
    }

    void evaluate_arden_lemma() noexcept{
        _rhs.release_brackets();
        _rhs.reorder(_lhs);
        _rhs.compactify_on_powers();



        _rhs.release_brackets();
        _rhs.reorder(_lhs);
        _rhs.compactify_on_powers();
    }

    void substitute(const Equation& substitution) noexcept{
        _rhs.substitute(substitution._lhs, substitution._rhs);
    }

    bool contain_nontermainals() const noexcept{
        return _rhs.contains_if([](const Regex& regex){
           return regex.node_type() == Regex::NONTERMINAL;
        });
    }

    inline GrammarUnit& lhs() noexcept { return _lhs; }
    inline const GrammarUnit& lhs() const noexcept { return _lhs; }
    inline const Regex& rhs() const noexcept { return _rhs; }
    inline Regex& rhs() noexcept { return _rhs; }

    friend std::ostream& operator<<(std::ostream& out, const Equation& equation){
        return out << equation._rhs.data() << " = " << equation._rhs;
    }
private:
    bool __arden_lemma_condition() const noexcept{
        bool is_independent_part_exist = false;
        bool is_dependent_part_exist = false;
        for(const auto& term: _rhs.children()) {
            bool find_result = term->contains_if([this](const Regex& regex) {
                return regex.node_type() == Regex::NONTERMINAL && regex.data() == _lhs.data();
            });
            is_independent_part_exist |= !find_result;
            is_dependent_part_exist |= find_result;
        }
        bool is_powers_different;
        return is_dependent_part_exist && is_independent_part_exist && is_powers_different;
    }
};