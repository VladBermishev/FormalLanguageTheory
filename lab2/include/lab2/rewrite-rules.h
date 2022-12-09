#pragma once
#include <vector>
#include <exception>
#include <common/string-extension.h>
#include <lab1/term-grammar.h>
#include <lab2/regex.h>
#include <lab2/rule.h>

class RewriteRules{
    typedef std::vector<Rule>::const_iterator const_iterator;
    const TermGrammar _grammar;
    std::vector<Rule> _rules;
public:
    explicit RewriteRules(TermGrammar grammar): _grammar(std::move(grammar)){}

    void push_back(Rule&& rule) {
        const TermGrammar lhs_grammar(rule.lhs().grammar([&grammar = _grammar](const char c){
            return (Regex::is_symbol(c)) ? (grammar.is_constant(c) ? 1 : 0) : -1;
        }));
        const TermGrammar rhs_grammar(rule.rhs().grammar([&grammar = _grammar](const char c){
            return (Regex::is_symbol(c)) ? (grammar.is_constant(c) ? 1 : 0) : -1;
        }));

        if (const auto grammar_difference = rhs_grammar.variables() - lhs_grammar.variables(); !grammar_difference.empty()){
            std::string invalid_arguments;
            bool temp{};
            grammar_difference.for_each_bit([&invalid_arguments, &temp](const uint64_t value){
                invalid_arguments += (temp ? ", " : (temp = 1, ""));
                invalid_arguments.push_back((char)value);
            });
            throw std::invalid_argument(string_format("Unexpected variable in rules rhs: %s",invalid_arguments.c_str()));
        }
        rule.variables() = lhs_grammar.variables();
        _rules.push_back(std::move(rule));
    }

    std::pair<const_iterator, Regex::RegexFindResult> find_rule(Regex& regex){
        for(auto it = _rules.cbegin(); it != _rules.cend(); it++)
            if (auto res = regex.find(it->lhs(), it->variables()); res.result != nullptr)
                return {it, res};
        return {_rules.cend(), Regex::RegexFindResult()};
    }
    const_iterator cend() { return _rules.cend(); }
};
