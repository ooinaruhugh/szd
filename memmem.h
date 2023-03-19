#ifndef _MEMMEM_H
#define _MEMMEM_H

#include <cstddef>

extern "C" void* memmem(const void* haystack, std::size_t haystacklen, const void* needle, std::size_t needlelen);

#endif // _MEMMEM_H