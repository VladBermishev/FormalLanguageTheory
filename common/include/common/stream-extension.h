#pragma once
#include <common/fxtbits-set.h>
#include <istream>
constexpr auto stream_max_size = std::numeric_limits<std::streamsize>::max();

bool is_alpha(const char ch) noexcept{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9');
}

template<typename _CharT, typename _Traits, std::uint8_t sz>
std::basic_istream<_CharT, _Traits>& lstrip(std::basic_istream<_CharT, _Traits>& in, const char (&delims)[sz]){
    if (in.eof()) { return in; }
    FxtBitsSet<256> delims_set;
    for(std::uint8_t idx = 0; idx < sz; idx++ ){ delims_set.insert(delims[idx]); }

    typedef typename _Traits::int_type int_type;
    const int_type __eof = _Traits::eof();
    std::basic_streambuf<_CharT,_Traits>* __sb = in.rdbuf();
    int_type __c = __sb->sgetc();
    while( !_Traits::eq_int_type(__c, __eof) &&
           delims_set.contains(__c))
    {
        __c = __sb->snextc();
    }
    return in;
}

template<typename CharT, typename Traits = std::char_traits<CharT> >
class basic_istream_extension : public std::basic_istream<CharT, Traits>{
    typedef typename Traits::int_type int_type;
public:

    explicit basic_istream_extension(std::basic_streambuf<CharT,Traits>* sb): std::basic_istream<CharT, Traits>(sb){}

    template<std::uint8_t sz>
    basic_istream_extension& lstrip(const char (&delims)[sz]){
        if (this->eof()) { return *this; }
        FxtBitsSet<256> delims_set;
        for(std::uint8_t idx = 0; idx < sz; idx++ ){ delims_set.insert(delims[idx]); }
        const int_type __eof = Traits::eof();
        std::basic_streambuf<CharT,Traits>* __sb = this->rdbuf();
        int_type __c = __sb->sgetc();
        while( !Traits::eq_int_type(__c, __eof) &&
               delims_set.contains(__c))
        {
            __c = __sb->snextc();
        }
        return *this;
    }
    basic_istream_extension& ignore(std::streamsize n, int_type delim){
        std::basic_istream<CharT, Traits>::ignore(n, delim);
        return *this;
    }

};
typedef basic_istream_extension<char> istream_extension;

