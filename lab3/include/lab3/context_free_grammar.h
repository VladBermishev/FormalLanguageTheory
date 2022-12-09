#pragma once
#include <common/fxtbits-set.h>
#include <common/stream-extension.h>
#include <lab3/grammar_unit.h>
#include <lab3/context_free_rule.h>
#include <string>
#include <unordered_set>

class ContextFreeGrammar{
    std::unordered_set<std::string> _terminals;
    std::unordered_set<std::string> _nonterminals;
    std::vector<ContextFreeRule> _rules;
public:
    ContextFreeGrammar(){}

    void add_rule(ContextFreeRule rule) noexcept{
        _nonterminals.insert(rule.lhs().data());
        for( const auto& unit: rule.rhs())
            if(unit.type() == GrammarUnit::NonTerminal)
                _nonterminals.insert(unit.data());
            else
                _terminals.insert(unit.data());
        _rules.push_back(std::move(rule));
    }

    ContextFreeGrammar& remove_epsilon_rules(){ return *this; }

    ContextFreeGrammar& remove_chain_rules(){ return *this; }

    inline const std::unordered_set<std::string>& terminals()const noexcept{ return _terminals; }

    inline const std::unordered_set<std::string>& nonterminals()const noexcept{ return _nonterminals; }

    inline const std::vector<ContextFreeRule>& rules() const noexcept { return _rules; }

    friend istream_extension& operator>>(istream_extension& in, ContextFreeGrammar& grammar) noexcept{
        while(!in.eof()){
            ContextFreeRule rule;
            in >> rule;
            grammar.add_rule(std::move(rule));
        }
    }
private:
};