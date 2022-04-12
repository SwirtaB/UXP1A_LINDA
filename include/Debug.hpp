#pragma once

#include <iostream>
#include <system_error>

template <typename... Args>
void debug(std::string &&message, Args... values) {
    std::cerr << message;
    [&](auto value) {
        std::cerr << " ";
        std::cerr << value;
    }(values...);
    std::cerr << std::endl;
}

template <typename Arg>
void debug(Arg &&s) {
    std::cerr << s << std::endl;
}
