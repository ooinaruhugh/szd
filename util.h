#include <cinttypes>

#ifndef _UTIL_H
#define _UTIL_H

inline uint16_t readUInt16LE(const char* bytes) {
    return ((unsigned)bytes[1] << 8) | ((unsigned)bytes[0]);
}

inline uint32_t readUInt32LE(const char* bytes) {
    return ((unsigned)bytes[3] << 24) 
           | ((unsigned)bytes[2] << 16) 
           | ((unsigned)bytes[1] << 8) 
           | ((unsigned)bytes[0]);
}

#endif // _UTIL_H
