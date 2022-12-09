#pragma once
#include <lab3/equation.h>
#include <unordered_set>
#include <algorithm>


class EquationsSystem {
    std::unordered_set<std::string> _terminals;
    std::unordered_set<std::string> _nonterminals;
    std::vector<Equation> _equations;
public:
    explicit EquationsSystem(const ContextFreeGrammar& grammar) {
        _terminals = grammar.terminals();
        _nonterminals = grammar.nonterminals();

        for (const auto& rule: grammar.rules()) {
            if (auto it = std::find_if(_equations.begin(),
                                       _equations.end(),
                                       [&rule](const Equation& eq) { return eq.lhs() == rule.lhs(); });
                    it != _equations.end())
                it->append(rule.rhs());
            else {
                _equations.emplace_back(rule.lhs());
                _equations.back().append(rule.rhs());
            }
        }

    }

    EquationsSystem& commutative_image() {
        bool flag = true;
        while(flag){
            flag = false;
            for(const auto& equation: _equations)
                flag |= equation.contain_nontermainals();
            for(std::uint32_t i = 0; i < _equations.size() && flag; i++){
                _equations[i].evaluate_arden_lemma();
                for(std::uint32_t j = 0; j < _equations.size(); j++)
                    if(i != j) _equations[j].substitute(_equations[i]);
            }
        }
    }

    auto begin() const noexcept { return _equations.cbegin(); }

    auto end() const noexcept { return _equations.end(); }
};