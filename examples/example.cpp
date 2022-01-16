// Copyright 2022 GNDavydov

#include <iostream>
#include <string>

#include <any.hpp>

using namespace my_any;

int main() {
    any x("123");
    any y(x);

    std::cout << any_cast<const char *>(x) << ' ' << any_cast<const char *>(y) << std::endl;
    y = 15;
    std::cout << any_cast<int>(y) << std::endl;

    any a;
    try {
        std::cout << a.has_value() << ' ';
        std::cout << any_cast<char>(a) << std::endl;
    }
    catch (const bad_any_cast &e) {
        std::cout << e.what() << std::endl;
    }

    a = y;
    y.emplace<std::string>(std::string("HI!"));
    std::cout << any_cast<int>(a) << ' ' << any_cast<std::string>(y) << std::endl;

    exit(EXIT_SUCCESS);
}

