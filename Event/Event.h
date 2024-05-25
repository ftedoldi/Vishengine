#pragma once

#include <vector>
#include <functional>

#define DECLARE_EVENT(class_name, ...) \
    typedef Event<__VA_ARGS__> class_name

template<typename... Args>
class Event {
public:
    using FunctionType = std::function<void(Args...)>;

    void AddFunction(FunctionType function) {
        _functions.emplace_back(std::move(function));
    }

    void Broadcast(Args... args) {
        for (const auto& function: _functions) {
            function(std::move(args)...);
        }
    }

private:
    std::vector<FunctionType> _functions;
};