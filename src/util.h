#ifndef _UTIL_H
#define _UTIL_H

#include <cstdint>
#include <cstddef>

enum class LogLevel {
    Info,
    Warning,
    Error
};

void Log(LogLevel level, const char *fmt, ...);

// TODO: Utility include
template<class T, size_t N>
constexpr size_t array_size(T(&)[N]) {
    return N;
}

#if defined(__linux__)
#include <strings.h>
#else
static int ffs(int value)
{
    int bit;

    if (value == 0)
        return 0;

    for (bit = 1; !(value & 1); bit++)
        value >>= 1;

    return bit;
}
#endif

#define GET_MASK(v, mask) (((v) & (mask)) >> (ffs(mask)-1))
#define SET_MASK(v, mask, val)                            \
    do {                                                  \
        const unsigned int __val = (val);                 \
        const unsigned int __mask = (mask);               \
        (v) &= ~(__mask);                                 \
        (v) |= ((__val) << (ffs(__mask) - 1)) & (__mask); \
    } while (0)

static inline uint64_t RangeGetLast(uint64_t offset, uint64_t len) {
    return offset + len - 1;
}

static inline int RangeCoversByte(uint64_t offset, uint64_t len, uint64_t byte) {
    return offset <= byte && byte <= RangeGetLast(offset, len);
}

static inline int RangesOverlap(uint64_t first1, uint64_t len1, uint64_t first2, uint64_t len2) {
    uint64_t last1 = RangeGetLast(first1, len1);
    uint64_t last2 = RangeGetLast(first2, len2);

    return !(last2 < first1 || last1 < first2);
}

#endif
