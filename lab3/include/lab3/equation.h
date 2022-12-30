#pragma once
#include <common/string-extension.h>
#include <lab3/grammar_unit.h>
#include <lab3/commutative_regex.h>
#include <iostream>
#include <list>
#include <vector>

class Equation{
    GrammarUnit _lhs;
    CommutativeRegex _rhs;
public:
    Equation() = default;

    Equation(GrammarUnit lhs) noexcept{
        _lhs = std::move(lhs);
    }

    void append(const std::list<GrammarUnit>& rule_rhs){
        CommutativeRegex term;
        if (rule_rhs.size() == 1){
            term.node_type() = (rule_rhs.front().type() == GrammarUnit::NonTerminal ? CommutativeRegex::NONTERMINAL : CommutativeRegex::TERMINAL);
            term.data() = rule_rhs.front().data();
        }else if( rule_rhs.size() > 1 ){
            term.node_type() = CommutativeRegex::CONCAT;
            term.data() = ".";
            for(const auto& unit: rule_rhs){
                const auto type = (unit.type() == GrammarUnit::NonTerminal ? CommutativeRegex::NONTERMINAL : CommutativeRegex::TERMINAL);
                term.push_back(CommutativeRegex(type, unit.data()));
            }
        }
        if(_rhs.node_type() == CommutativeRegex::EMPTY)
            _rhs = std::move(term);
        else if (_rhs.node_type() == CommutativeRegex::ALTERNATIVE){
            _rhs.push_back(std::move(term));
        }else{
            CommutativeRegex buffer(CommutativeRegex::ALTERNATIVE, "+");
            buffer.push_back(std::move(_rhs));
            buffer.push_back(std::move(term));
            _rhs = std::move(buffer);
        }
    }

    void evaluate_arden_lemma(){
        _rhs.relax(_lhs);
        auto [independent, dependent] = _rhs.distribute_variable(_lhs);
        if(dependent.node_type() != CommutativeRegex::EMPTY) {
            dependent.decrease_power(_lhs);
            dependent.substitute(_lhs, independent);
            CommutativeRegex buffer = CommutativeRegex(CommutativeRegex::STAR, "*");
            buffer.push_back(std::move(dependent));
            _rhs = CommutativeRegex(CommutativeRegex::CONCAT, ".");
            _rhs.push_back(std::move(buffer));
            _rhs.push_back(std::move(independent));
        }else if(dependent.node_type() == CommutativeRegex::EMPTY && independent.node_type() != CommutativeRegex::EMPTY) {
            _rhs = std::move(independent);
        }
    }

    void substitute(const Equation& substitution) noexcept{
        _rhs.substitute(substitution._lhs, substitution._rhs);
        _rhs.relax(_lhs);
    }

    bool contain_nontermainals() const noexcept{
        return _rhs.contains_if([](const CommutativeRegex& regex){
           return regex.node_type() == CommutativeRegex::NONTERMINAL;
        });
    }

    inline GrammarUnit& lhs() noexcept { return _lhs; }
    inline const GrammarUnit& lhs() const noexcept { return _lhs; }
    inline const CommutativeRegex& rhs() const noexcept { return _rhs; }
    inline CommutativeRegex& rhs() noexcept { return _rhs; }

    friend std::ostream& operator<<(std::ostream& out, const Equation& equation){
        return out << equation._lhs.data() << " = " << equation._rhs;
    }
};