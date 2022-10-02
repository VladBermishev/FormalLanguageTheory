#pragma once
#include <chrono>
#include <iostream>
#include <string>

class Perf{
    const std::chrono::time_point<std::chrono::steady_clock> _start;
    const std::string _title;
public:
    explicit Perf(const std::string& title ):
            _start(std::chrono::steady_clock::now()),
            _title(title)
    {}

    ~Perf(){
        std::cout << _title << ": "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - _start).count()
                  << "ns\n";
        std::cout.flush();
    }
};
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#if defined(DEBUG) or defined(NDEBUG)
#define PERF(title) Perf CONCAT(p, __LINE__ ) (title);
#else
#define PERF(title)
#endif