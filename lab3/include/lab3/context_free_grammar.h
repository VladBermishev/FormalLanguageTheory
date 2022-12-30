#pragma once
#include <common/fxtbits-set.h>
#include <common/logger.h>
#include <common/stream-extension.h>
#include <lab3/grammar_unit.h>
#include <lab3/context_free_rule.h>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>

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

    ContextFreeGrammar& remove_epsilon_rules(){
        std::vector<ContextFreeRule> new_rules;
        const auto epsilon_reproducing_nonterminals = __epsilon_reproducing_nonterminals();
        if (epsilon_reproducing_nonterminals.size())
            LOG("Folowing nonterminals are deleted as epsilon producing rules:", epsilon_reproducing_nonterminals, '\n');
        for(const auto& rule: _rules){
            if(epsilon_reproducing_nonterminals.find(rule.lhs().data()) == epsilon_reproducing_nonterminals.end()){
                ContextFreeRule new_rule;
                new_rule.lhs() = rule.lhs();
                for(const auto& nonterminal: rule.rhs())
                    if(epsilon_reproducing_nonterminals.find(nonterminal.data()) == epsilon_reproducing_nonterminals.end())
                        new_rule.rhs().push_back(nonterminal);
                if(!new_rule.rhs().empty())
                    new_rules.push_back(std::move(new_rule));
            }
        }
        _rules = new_rules;
        return *this;
    }

    ContextFreeGrammar& remove_chain_rules(){
        std::vector<ContextFreeRule> new_rules;
        const auto chain_pairs = __chain_pairs();

        for(const auto& chain_pair: chain_pairs){
            for(const auto& rule: _rules){
                if(rule.lhs().data() == chain_pair.second && ( rule.rhs().size() != 1 ||
                   (rule.rhs().begin()->type() != GrammarUnit::NonTerminal &&
                   chain_pairs.find(std::make_pair(chain_pair.second,rule.rhs().begin()->data())) == chain_pairs.end())))
                {
                    ContextFreeRule new_rule;
                    new_rule.lhs() = GrammarUnit(GrammarUnit::NonTerminal,chain_pair.first);
                    new_rule.rhs() = rule.rhs();
                    new_rules.push_back(std::move(new_rule));
                }
            }
        }
        _rules = new_rules;
        return *this;
    }

    inline std::vector<ContextFreeRule> rules() const noexcept { return _rules; }
    inline const std::unordered_set<std::string>& nonterminals() const noexcept { return _nonterminals; }

    friend istream_extension& operator>>(istream_extension& in, ContextFreeGrammar& grammar) noexcept{
        while(!in.eof()){
            ContextFreeRule rule;
            in >> rule;
            grammar.add_rule(std::move(rule));
        }
    }
private:

    /*
     * Epsilon reproducing nonterminals are following:
     *      A -> eps
     *      B -> eps
     *      C -> AB
     * Other nonterminals, such as:
     *      A -> a
     *      A -> eps
     *      B -> eps
     *      B -> AAb
     *      C -> AB
     * Will be handled correctly in Equation::evaluate_arden_lemma
     */
    std::unordered_set<std::string> __epsilon_reproducing_nonterminals(){
        std::unordered_set<std::string> result;
        std::unordered_map<std::string, std::uint32_t> nonterminal_alternatives;
        for(const auto& rule: _rules)
            nonterminal_alternatives[rule.lhs().data()]++;

        for(const auto& rule: _rules)
            if(rule.rhs().empty() && nonterminal_alternatives[rule.lhs().data()] == 1)
                result.insert(rule.lhs().data());
        bool is_result_changed = true;
        while (is_result_changed) {
            is_result_changed = false;
            for (std::uint32_t idx = 0; idx < _rules.size(); idx++) {
                if(result.find(_rules[idx].lhs().data()) != result.end()) continue;
                bool is_reproducing_epsilon = !_rules[idx].rhs().empty();
                for (const auto& nonterminal: _rules[idx].rhs())
                    is_reproducing_epsilon &= (result.find(nonterminal.data()) != result.end());
                if (is_reproducing_epsilon){
                    result.insert(_rules[idx].lhs().data());
                    is_result_changed = true;
                }
            }
        }
        return result;
    }
    std::set<std::pair<std::string,std::string>> __chain_pairs(){
        std::set<std::pair<std::string,std::string>> result;
        for(const auto& rule: _rules)
            result.emplace(rule.lhs().data(),rule.lhs().data());
        bool is_result_changed = true;
        while(is_result_changed){
            is_result_changed = false;
            for(const auto& chain_pair: result){
                for(const auto& rule: _rules){
                    if(rule.lhs().data() == chain_pair.second && rule.rhs().size() == 1 &&
                       rule.rhs().begin()->type() == GrammarUnit::NonTerminal &&
                       result.find(std::make_pair(chain_pair.first,rule.rhs().begin()->data())) == result.end()){
                        result.insert(std::make_pair(chain_pair.first,rule.rhs().begin()->data()));
                        is_result_changed = true;
                    }
                }
            }
        }
        return result;
    }
};