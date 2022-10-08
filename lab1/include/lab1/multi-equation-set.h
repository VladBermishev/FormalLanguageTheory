#pragma once
#include <vector>
#include <queue>
#include <ostream>
#include <lab1/multi-equation.h>


class MultiEquationSet {
    typedef typename std::vector<MultiEquation>::const_iterator const_iterator;
    std::vector<MultiEquation> _data;
public:
    MultiEquationSet() = default;

    [[nodiscard]] inline bool empty() const noexcept { return _data.empty(); }

    void insert(MultiEquation&& value) noexcept {
        MultiEquation res(value);
        for (auto it = _data.begin(); it != _data.end();) {
            if (!it->intersect(value).empty()) {
                res.merge(std::move(*std::make_move_iterator(it)));
                _data.erase(it);
            } else {
                it++;
            }
        }
        _data.push_back(res);
    }

    void insert(const MultiEquation& value) noexcept {
        MultiEquation res(value);
        for (auto it = _data.begin(); it != _data.end();) {
            if (!it->intersect(value).empty()) {
                res.merge(*it);
                _data.erase(it);
            } else {
                it++;
            }
        }
        _data.push_back(res);
    }

    void erase(const_iterator it) { _data.erase(it); }

    void merge(const MultiEquationSet& rhs) { for (const auto& multeq: rhs._data) insert(multeq); }

    void merge(MultiEquationSet&& rhs) { for (auto&& multeq: rhs._data) insert(std::move(multeq)); }

    inline const_iterator cend() const noexcept { return _data.cend(); }

    [[nodiscard]] const_iterator unique() const noexcept {
        for (auto iter = _data.cbegin(); iter != _data.cend(); iter++)
            if (is_unique(iter)) return iter;
        return _data.cend();
    }

    Term term() const noexcept{
        Term res;
        if (_data.empty()) return res;
        auto it = _data.cbegin();
        while(it != _data.cend() && !it->lhs().contains(0)) it++;
        if (it == _data.cend()){ return res; }
        res = **it->rhs().begin();
        std::queue<Term*> term_queue;
        term_queue.push(&res);
        while (!term_queue.empty()){
            auto node = term_queue.front();
            term_queue.pop();
            if (!node) continue;
            for(auto& tptr: node->children()){
                if(auto it = find(tptr->name()); tptr->is_variable() && it != _data.cend()){
                    tptr = new Term();
                    *tptr = **it->rhs().begin();
                }else{
                    const auto __tmp = new Term();
                    *__tmp = *tptr;
                    tptr = __tmp;
                }
                term_queue.push(tptr);
            }
        }
        return res;
    }

    void free() {
        for (auto& multeq: _data) { multeq.free(); }
    }

    friend std::ostream& operator<<(std::ostream& out, const MultiEquationSet& multiEquationSet) noexcept {
        for (const auto& multeq: multiEquationSet._data) { out << multeq << '\n'; }
        return out;
    }

private:
    bool is_unique(const_iterator it) const noexcept {
        bool res = true;
        for (auto iter = _data.cbegin(); iter != _data.cend() && res; iter++) {
            if (iter == it) continue;
            res &= (iter->rhs_variables() & it->lhs()).empty();
        }
        return res;
    }
    const_iterator find(const char value) const noexcept{
        for(auto it = _data.cbegin(); it != _data.cend(); it++){if (it->lhs().contains(value)) return it;}
        return _data.cend();
    }

};