#pragma once
#include <common/fxtbits-set.h>
#include <common/stream-extension.h>
#include <lab3/context_free_rule.h>
#include <lab3/grammar_unit.h>
#include <string>
#include <vector>
#include <list>

class ContextFreeRule{
    GrammarUnit _lhs;
    std::list<GrammarUnit> _rhs;
public:
    ContextFreeRule() = default;

    /*
     * Cringe :(
     */
    friend istream_extension& operator>>(istream_extension& in, ContextFreeRule& grammar) noexcept{
        grammar._lhs = GrammarUnit(GrammarUnit::NonTerminal, read_nterm(in));
        in.ignore(stream_max_size, '>').lstrip("\t ");
        while(in.peek() != '\n' && in.peek() != '\r' && !in.eof())
            if(in.peek() == '[')
                grammar._rhs.emplace_back(GrammarUnit::NonTerminal, read_nterm(in));
            else
                grammar._rhs.emplace_back(GrammarUnit::Terminal, read_term(in));
        return in;
    }

    inline const GrammarUnit& lhs() const noexcept{ return _lhs; }
    inline const std::list<GrammarUnit>& rhs() const noexcept{ return _rhs; }

private:
    static std::string read_nterm(istream_extension& in) noexcept{
        std::string result;
        in.ignore(stream_max_size,'[');
        char to_push;
        while((to_push = in.get()) != ']')
            result.push_back(to_push);
        return result;
    }

    static std::string read_term(istream_extension& in) noexcept{
        std::string result;
        if (!in.eof())
            result.push_back(in.get());
        return result;
    }
};