#include "util.h"
#include <iostream>
#include <iomanip>

void printByteBuffer(const std::vector<char> buffer) {
    for (unsigned char c : buffer) {
	auto f{std::cout.flags()};

        std::cout << std::setfill('0') << std::setw(2) << std::hex << +c << ' ';
            
        std::cout.flags(f);
    }
    std::cout << std::endl;
}

void printByteBuffer(const char* buffer, size_t n) {
    while (n-- > 0) {
        auto f{std::cout.flags()};

        std::cout << std::setfill('0') << std::setw(2) << std::hex << +*(buffer++) << ' ';
            
        std::cout.flags(f);
    }
    std::cout << std::endl;
}