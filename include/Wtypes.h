#pragma once

// ----- unsigned -----
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

// ----- signed -----
typedef char       i8;
typedef short      i16;
typedef int        i32;
typedef long long  i64;

// ----- floating point ------
typedef float  f32;
typedef double f64;

// ----- boolean  ------------
typedef bool boolean;

// ----- size_t and uintptr_t -----
#ifdef _WIN64
    typedef unsigned long long size_t;
    typedef signed long long   ssize_t;
    typedef unsigned long long uintptr_t;
#else 
    typedef unsigned int size_t;
    typedef signed int   ssize_t;
    typedef unsigned int uintptr_t;
#endif

#if defined (__GNUC__) || defined(__clang__)
    typedef __SIZE_TYPE__ size_t;
#elif defined(_MSC_VER)
    typedef unsigned __int64 size_t;
#else
    typedef unsigned long size_t;
#endif

// ----- win types -----
typedef void *HANDLE;
typedef void *HWND;
typedef void *HINSTANCE;
typedef u32   DWORD;
typedef i32   BOOL;
