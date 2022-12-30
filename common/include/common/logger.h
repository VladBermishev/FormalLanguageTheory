#include <iostream>
#include <unordered_set>

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::unordered_set<T>& set){
    out << "{ ";
    bool flag{};
    for(const auto& elem: set)
        out << (flag ? ", " + elem : (flag = 1, elem) );
    return out << " }";
}

#ifdef DEBUG
template<typename ...Args>
void LOG(Args&& ...args){
    (std::clog << ... << args);
}
#else
template<typename ...Args>
void LOG(Args&& ...args){}
#endif