#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <common/stream-extension.h>
#include <common/perf.h>
#include <lab1/multi-equation-set.h>




std::pair<Term*, MultiEquationSet> dec(const MultiEquation::term_set_type& terms) {
    const Term* variable_in_rhs_terms = nullptr;
    bool __root_name_matches = true;
    const char __root_name = (*terms.begin())->name();
    for (const auto tptr: terms) {
        __root_name_matches &= tptr->name() == __root_name;
        if (tptr->is_variable()) { variable_in_rhs_terms = tptr; }
    }
    if (variable_in_rhs_terms) {
        // make-multeq
        MultiEquationSet s;
        MultiEquation multeq;
        for (const auto tptr: terms) {
            if (tptr->is_variable())
                multeq.insert_lhs(tptr->name());
            else
                multeq.insert_rhs(tptr);
        }
        s.insert(std::move(multeq));
        return {new Term(variable_in_rhs_terms->name(),Term::TermType::VARIABLE), std::move(s)};
    } else if (__root_name_matches) {
        Term* common_term = new Term(__root_name);
        MultiEquationSet frontier;
        MultiEquation::term_set_type buffer;
        for (std::uint64_t idx = 0; idx < (*terms.begin())->size(); idx++) {
            for (const auto tptr: terms){
                buffer.insert(tptr->children()[idx]);
            }

            auto [__common_term, __frontier] = dec(buffer);
            if (__common_term == nullptr) {
                delete common_term;
                return {nullptr, std::move(frontier)};
            }
            common_term->insert_child(__common_term);
            frontier.merge(std::move(__frontier));
            buffer.clear();
        }
        return {common_term, std::move(frontier)};
    } else {
        return {nullptr, MultiEquationSet()};
    }
}

enum UnificationResult {OK = 0, TERMS_UNDEFINED, UNIQUE_MULTIEQUATION_NOT_FOUND, COMMON_TERM_NOT_FOUND};
UnificationResult unify(const Term& lhs, const Term& rhs, MultiEquationSet& result) {
    PERF("UNIFY")
    MultiEquationSet initial_set;
    if (lhs.variables().empty() || rhs.variables().empty())
        return UnificationResult::TERMS_UNDEFINED;
    lhs.variables().for_each_bit([&initial_set](const std::uint64_t value) {
        initial_set.insert(MultiEquation({value}));
    });
    rhs.variables().for_each_bit([&initial_set](const std::uint64_t value) {
        initial_set.insert(MultiEquation({value}));
    });
    initial_set.insert(MultiEquation({0}, {&lhs, &rhs}));
    while (!initial_set.empty()) {
        const auto unique_equation = initial_set.unique();
        if (unique_equation == initial_set.cend())
            return UnificationResult::UNIQUE_MULTIEQUATION_NOT_FOUND;
        if (unique_equation->rhs().empty()) {
            result.insert(MultiEquation(unique_equation->lhs(), {}));
            initial_set.erase(unique_equation);
        } else {
            auto [common_term, frontier] = dec(unique_equation->rhs());
            if (common_term == nullptr) return UnificationResult::COMMON_TERM_NOT_FOUND;
            result.insert(MultiEquation(unique_equation->lhs(), {common_term}));
            initial_set.erase(unique_equation);
            initial_set.merge(std::move(frontier));
        }
    }
    return UnificationResult::OK;
}

int main(int argc, char** argv) {
    PERF("main")
    istream_extension input(std::cin.rdbuf());
    std::ifstream finput;
    if (argc == 2) {
        if (std::filesystem::exists(argv[1])) {
            finput.open(argv[1]);
            input.rdbuf(finput.rdbuf());
        } else {
            throw std::runtime_error("File wasn't found at given filepath");
        }
    }
    Grammar grammar;
    input.lstrip("\r\n\t ").ignore(stream_max_size, '=').lstrip(" ");
    grammar.parse_constructors(input);
    input.lstrip("\r\n\t ").ignore(stream_max_size, '=').lstrip(" ");
    grammar.parse_variables(input);
    Term first_term;
    input.lstrip("\r\n\t ").ignore(stream_max_size, ':').lstrip(" ") >> first_term;
    Term second_term;
    input.lstrip("\r\n\t ").ignore(stream_max_size, ':').lstrip(" ") >> second_term;
    first_term.identify(grammar); second_term.identify(grammar);
    MultiEquationSet multiEquationSet;
    const auto res = unify(first_term, second_term, multiEquationSet);
    if (res == UnificationResult::OK) {
        PERF("PRINT")
        std::cout << multiEquationSet;
        std::cout << multiEquationSet.term() << '\n';
    } else {
        std::cout << "not unified\n";
    }
    multiEquationSet.free();
    return 0;
}