// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#ifndef _COMMONFUNCS_H_
#define _COMMONFUNCS_H_

#include <future>
#include <thread>
#include <chrono>

// iOS-compatible SLEEP macro using std::this_thread
#define SLEEP(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

template <bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert<true> {};

#define b2(x)   (   (x) | (   (x) >> 1) )
#define b4(x)   ( b2(x) | ( b2(x) >> 2) )
#define b8(x)   ( b4(x) | ( b4(x) >> 4) )
#define b16(x)  ( b8(x) | ( b8(x) >> 8) )  
#define b32(x)  (b16(x) | (b16(x) >>16) )
#define ROUND_UP_POW2(x)    (b32(x - 1) + 1)

#ifndef MIN
#define MIN(a, b)   ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a, b)   ((a)>(b)?(a):(b))
#endif

#define CLAMP(x, min, max)  (((x) > max) ? max : (((x) < min) ? min : (x)))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifndef DECLARE_ENUM_FLAG_OPERATORS
#define DECLARE_ENUM_FLAG_OPERATORS(T)                                                            \
    inline constexpr T operator|(T lhs, T rhs) {                                                  \
        using U = std::underlying_type_t<T>;                                                      \
        return static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs));                         \
    }                                                                                              \
    inline constexpr T operator&(T lhs, T rhs) {                                                  \
        using U = std::underlying_type_t<T>;                                                      \
        return static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs));                         \
    }                                                                                              \
    inline constexpr T& operator|=(T& lhs, T rhs) {                                               \
        lhs = lhs | rhs;                                                                           \
        return lhs;                                                                                \
    }
#endif

#ifndef DECLARE_ENUM_ARITHMETIC_OPERATORS
#define DECLARE_ENUM_ARITHMETIC_OPERATORS(T)                                                      \
    inline constexpr T operator+(T lhs, int rhs) {                                                \
        using U = std::underlying_type_t<T>;                                                      \
        return static_cast<T>(static_cast<U>(lhs) + rhs);                                         \
    }                                                                                              \
    inline constexpr T& operator+=(T& lhs, int rhs) {                                             \
        lhs = lhs + rhs;                                                                           \
        return lhs;                                                                                \
    }
#endif

#include <errno.h>
#include <byteswap.h>

// go to debugger mode
    #define Crash() {__builtin_trap();}
    #define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
// GCC 4.8 defines all the rotate functions now
// Small issue with GCC's lrotl/lrotr intrinsics is they are still 32bit while we require 64bit
#ifndef _rotl
inline u32 _rotl(u32 x, int shift) {
    shift &= 31;
    if (!shift) return x;
    return (x << shift) | (x >> (32 - shift));
}

inline u32 _rotr(u32 x, int shift) {
    shift &= 31;
    if (!shift) return x;
    return (x >> shift) | (x << (32 - shift));
}
#endif

inline u64 _rotl64(u64 x, unsigned int shift){
    unsigned int n = shift % 64;
    return (x << n) | (x >> (64 - n));
}

inline u64 _rotr64(u64 x, unsigned int shift){
    unsigned int n = shift % 64;
    return (x >> n) | (x << (64 - n));
}

// Dolphin's min and max functions
#undef min
#undef max

template<class T>
inline T min(const T& a, const T& b) {return a > b ? b : a;}
template<class T>
inline T max(const T& a, const T& b) {return a > b ? a : b;}

// Generic function to get last error message.
// Call directly after the command or use the error num.
// This function might change the error code.
// Defined in Misc.cpp.
const char* GetLastErrorMsg();

namespace Common
{
inline u8 swap8(u8 _data) {return _data;}
inline u32 swap24(const u8* _data) {return (_data[0] << 16) | (_data[1] << 8) | _data[2];}

inline __attribute__((always_inline)) u16 swap16(u16 _data)
    {return (_data >> 8) | (_data << 8);}
inline __attribute__((always_inline)) u32 swap32(u32 _data)
    {return __builtin_bswap32(_data);}
inline __attribute__((always_inline)) u64 swap64(u64 _data)
    {return __builtin_bswap64(_data);}

inline u16 swap16(const u8* _pData) {return swap16(*(const u16*)_pData);}
inline u32 swap32(const u8* _pData) {return swap32(*(const u32*)_pData);}
inline u64 swap64(const u8* _pData) {return swap64(*(const u64*)_pData);}

template <int count>
void swap(u8*);

template <>
inline void swap<1>(u8* data)
{}

template <>
inline void swap<2>(u8* data)
{
    *reinterpret_cast<u16*>(data) = swap16(data);
}

template <>
inline void swap<4>(u8* data)
{
    *reinterpret_cast<u32*>(data) = swap32(data);
}

template <>
inline void swap<8>(u8* data)
{
    *reinterpret_cast<u64*>(data) = swap64(data);
}

template <typename T>
inline T FromBigEndian(T data)
{
    //static_assert(std::is_arithmetic<T>::value, "function only makes sense with arithmetic types");
    
    swap<sizeof(data)>(reinterpret_cast<u8*>(&data));
    return data;
}

}  // Namespace Common

#endif // _COMMONFUNCS_H_
