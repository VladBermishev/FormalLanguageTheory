#pragma once
#include <lab3/context_free_grammar.h>
#include <lab3/equation.h>
#include <unordered_set>
#include <algorithm>


class EquationsSystem {
    std::vector<Equation> _equations;
public:
    explicit EquationsSystem(const ContextFreeGrammar& grammar) {
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
        if(grammar.nonterminals().size() != _equations.size())
            throw std::logic_error("Some nonterminals aren't defined");
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
        for(std::uint32_t i = 0; i < _equations.size(); i++)
            _equations[i].rhs().relax(_equations[i].lhs());
        return *this;
    }

    auto begin() const noexcept { return _equations.cbegin(); }

    auto end() const noexcept { return _equations.end(); }
};