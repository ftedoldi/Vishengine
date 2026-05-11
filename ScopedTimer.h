#pragma once

#include <chrono>
#include <iostream>

class ScopedTimer {
public:
    ScopedTimer(const char* name) : _name{name}, _start{std::chrono::steady_clock::now()} {}
    ~ScopedTimer() {
        const auto end = std::chrono::steady_clock::now();
        const auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count();
        std::cout << _name << ": " << us << " us\n";
    }
private:
    const char* _name;
    std::chrono::steady_clock::time_point _start;
};

#define PROFILE_SCOPE(name) ScopedTimer _t_##__LINE__{name}