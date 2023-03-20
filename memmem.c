#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

typedef uint8_t BYTE;

void *memchr(const void* s, int c, size_t n) {
    if (n == 0) return NULL;

    const BYTE *bs = (BYTE*)s;

    for (int i = 0; i < n; i++) {
        if (bs[i] == c) return (void*)(bs+i);
    }
    return NULL;		
}

void* memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen) {
    if (haystacklen == 0 || needlelen == 0) return NULL;
    if (needlelen > haystacklen) return NULL;
    if (needlelen == 1) return memchr(haystack, *(BYTE*)needle, haystacklen);

    BYTE *last = (BYTE*)haystack + haystacklen - needlelen;
    BYTE *bneedle = (BYTE*)needle;

    for (BYTE* i = (BYTE*)haystack; i < last; i++) {
        bool found = true;
        for (int ii = 0; ii < needlelen; ii++) {
            if (i[ii] != bneedle[ii]) {
                found = false;
                break;
            }
        }

        if (found) return i;
    }

    return NULL;
}