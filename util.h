#include <cstddef>
#include <cinttypes>

#include <vector>
#include <array>

#ifndef _UTIL_H
#define _UTIL_H

using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;

/*
    Reading little-endian bytes in a host-agnostic way.
*/

inline WORD getWordLE(const unsigned char* bytes) {
    return ((WORD)bytes[1] << 8) | ((WORD)bytes[0]);
}

inline DWORD getDWordLE(const unsigned char* bytes) {
    return ((DWORD)bytes[3] << 24) 
           | ((DWORD)bytes[2] << 16) 
           | ((DWORD)bytes[1] << 8) 
           | ((DWORD)bytes[0]);
}

inline QWORD getQWordLE(const unsigned char* bytes) {
    return ((QWORD)bytes[7] << 56)
           | ((QWORD)bytes[6] << 48) 
           | ((QWORD)bytes[5] << 40) 
           | ((QWORD)bytes[4] << 32) 
           | ((QWORD)bytes[3] << 24) 
           | ((QWORD)bytes[2] << 16) 
           | ((QWORD)bytes[1] << 8) 
           | ((QWORD)bytes[0]);
}

/*
    Writing bytes as little-endian in an host-agnostic way.
*/

inline std::array<char, sizeof(WORD)> putWordLE(WORD bytes) {
    return {
        (char)(bytes & 0xFF),
        (char)((bytes & 0xFF00) >> 8)
    };
}

inline std::array<char, sizeof(DWORD)> putDWordLE(DWORD bytes) {
    return {
        (char)(bytes & 0xFF),
        (char)((bytes & 0xFF00) >> 8),
        (char)((bytes & 0xFF0000) >> 16),
        (char)((bytes & 0xFF000000) >> 24)
    };
}

/*
    This append the contents of some container types into a std::vector.
*/
template<typename T>
inline void appendVectorToVector(std::vector<T>& to, std::vector<T> from) {
    to.insert(to.end(), from.cbegin(), from.cend());
}

template<typename T, size_t size>
inline void appendArrayToVector(std::vector<T>& to, std::array<T, size> from) {
    to.insert(to.end(), from.cbegin(), from.cend());
}

/*
    Prints the content of the specified byte buffer, as hex bytes.
*/
void printByteBuffer(const char* buffer, size_t n);
void printByteBuffer(const std::vector<char> buffer);

#endif // _UTIL_H
