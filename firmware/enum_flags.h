#pragma once

#include <stdint.h>

// Source: https://cplusplus.com/forum/general/44137/
#ifdef __cplusplus

// Define operator overloads to enable bit operations on enum values that are 
// used to define flags. Use DEFINE_ENUM_FLAG_OPERATORS(YOUR_TYPE) to enable these 
// operators on YOUR_TYPE.

// Templates are defined here in order to avoid a dependency on C++ <type_traits> header file,
// or on compiler-specific contructs.
extern "C++" {

    template <size_t S>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE;

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<1>
    {
        typedef int8_t type;
    };

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<2>
    {
        typedef int16_t type;
    };

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<4>
    {
        typedef int32_t type;
    };

    // used as an approximation of std::underlying_type<T>
    template <class T>
    struct _ENUM_FLAG_SIZED_INTEGER
    {
        typedef typename _ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
    };

}

#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE clear_enum_flag(ENUMTYPE original, ENUMTYPE flag) { return original & (~flag); } \
inline ENUMTYPE set_enum_flag(ENUMTYPE original, ENUMTYPE flag) { return original | flag; } \
inline ENUMTYPE toggle_enum_flag(ENUMTYPE original, ENUMTYPE flag) { return original ^ flag; } \
}
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif