#ifndef _UTIL_H
#define _UTIL_H

#include <cstddef>
#include <cinttypes>

#include <vector>
#include <array>

using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;

inline bool nthBitIsSet(QWORD bitfield, unsigned short bit) {
    return bitfield & (QWORD(1) << bit);
}

/*
    Reading little-endian bytes in a host-agnostic way.
*/

/// Reads a `WORD` from a little-endian byte array in a host-agnostic way.
/// - Parameter bytes: A little-endian byte array
inline WORD getWordLE(const unsigned char* bytes) {
    return ((WORD)bytes[1] << 8) | ((WORD)bytes[0]);
}

/// Reads a `DWORD` from a little-endian byte array in a host-agnostic way.
/// - Parameter bytes: A little-endian byte array
inline DWORD getDWordLE(const unsigned char* bytes) {
    return ((DWORD)bytes[3] << 24) 
           | ((DWORD)bytes[2] << 16) 
           | ((DWORD)bytes[1] << 8) 
           | ((DWORD)bytes[0]);
}

/// Reads a `QWORD` from a little-endian byte array in a host-agnostic way.
/// - Parameter bytes: A little-endian byte array
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

/// Writes a `WORD` given as number into a little-endian byte array in a host-agnostic way
/// - Parameter bytes: A number describing a   `DWORD` (a.k.a. a `uint16_t`)
inline std::array<char, sizeof(WORD)> putWordLE(WORD bytes) {
    return {
        (char)(bytes & 0xFF),
        (char)((bytes & 0xFF00) >> 8)
    };
}

/// Writes a `DWORD` given as number into a little-endian byte array in a host-agnostic way
/// - Parameter bytes: A number describing a   `DWORD` (a.k.a. a `uint32_t`)
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

/// This append the contents of a `std::vector` to a `std::vector`.
/// - Parameters:
///   - to: The target `std::vector`
///   - from: The donor `std::vector`
template<typename T> inline void appendVectorToVector(std::vector<T>& to, std::vector<T> from) {
    to.insert(to.end(), from.cbegin(), from.cend());
}

/// This append the contents of a `std::array` to a `std::vector`.
/// - Parameters:
///   - to: The target `std::vector`
///   - from: The donor `std::array`
template<typename T, size_t size> inline void appendArrayToVector(std::vector<T>& to, std::array<T, size> from) {
    to.insert(to.end(), from.cbegin(), from.cend());
}

/*
    Prints the content of the specified byte buffer, as hex bytes.
*/
void printByteBuffer(const char* buffer, size_t n);
void printByteBuffer(const std::vector<char> buffer);

#endif // _UTIL_H
