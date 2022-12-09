#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <cstring>


std::vector<std::string_view> split(const char* s, const uint64_t len, const char separator) {
    std::vector<std::string_view> res;
    const char* _first = s;
    const char* _second = _first;
    const char* const _last = s + len;
    while (_first < _last){
        _second = strchr(_first,separator);
        if( _second == nullptr ){
            res.emplace_back(_first, _last - _first);
            break;
        }
        if( _first != _second){ res.emplace_back(_first,_second - _first); }
        _first = _second + 1;
    }
    return res;
}

template<std::uint64_t sz>
std::vector<std::string_view> tokenize(const char* s, const uint64_t len, const char (&separators)[sz] ){
    return {};
}

std::string join(std::vector<std::string>::const_iterator b, std::vector<std::string>::const_iterator e, const char separator) {
    std::string res = *(b++);
    while (b != e) {
        res += separator;
        res += *(b++);
    }
    return res;
}

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ){
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
