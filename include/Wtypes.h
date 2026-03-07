/*
 * Copyright 2026 Jas2365
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

// ----- integer limits -----
#define min_i8   (-128)
#define max_i8   (127)
#define min_u8   (0)
#define max_u8   (255)

#define min_i16  (-32768)
#define max_i16  (32767)
#define min_u16  (0)
#define max_u16  (65535)

#define min_i32  (-2147483648)
#define max_i32  (2147483647)
#define min_u32  (0)
#define max_u32  (4294967295U)

#define min_i64  (-9223372036854775807LL - 1)
#define max_i64  (9223372036854775807LL)
#define min_u64  (0)
#define max_u64  (18446744073709551615ULL)

// ----- floating point limits -----
#define min_f32  (-3.402823e38f)
#define max_f32  (3.402823e38f)
#define min_f64  (-1.797639e308)
#define max_f64  (1.797639e308)

// ----- boolean  ------------
typedef bool boolean;

// ----- size_t and uintptr_t -----
#ifdef _WIN64
    typedef unsigned long long   size_t;
    typedef signed   long long   ssize_t;
    typedef unsigned long long   uintptr_t;
    typedef signed   long long   intptr_t;
    typedef signed   long long   ptrdiff_t;
    
    #define min_size_t   (0)
    #define max_size_t   (18446744073709551615ULL)
    #define min_ssize_t  (-9223372036854775807LL - 1)
    #define max_ssize_t  (9223372036854775807LL)
    #define min_uintptr_t (0)
    #define max_uintptr_t (18446744073709551615ULL)
    #define min_intptr_t (-9223372036854775807LL - 1)
    #define max_intptr_t (9223372036854775807LL)
    #define min_ptrdiff_t (-9223372036854775807LL - 1)
    #define max_ptrdiff_t (9223372036854775807LL)
#elif _WIN32
    typedef unsigned int size_t;
    typedef signed int   ssize_t;
    typedef unsigned int uintptr_t;
    typedef signed int   intptr_t;
    typedef signed int   ptrdiff_t;
    
    #define min_size_t   (0)
    #define max_size_t   (4294967295U)
    #define min_ssize_t  (-2147483648)
    #define max_ssize_t  (2147483647)
    #define min_uintptr_t (0)
    #define max_uintptr_t (4294967295U)
    #define min_intptr_t (-2147483648)
    #define max_intptr_t (2147483647)
    #define min_ptrdiff_t (-2147483648)
    #define max_ptrdiff_t (2147483647)
#endif

#if defined (__GNUC__) || defined(__clang__)
    typedef __SIZE_TYPE__ size_t;
#elif defined(_MSC_VER)
    typedef unsigned __int64 size_t;
#else
    typedef unsigned long size_t;
#endif

// ----- additional types -----
typedef unsigned short wchar_t;
typedef long long intmax_t;
typedef unsigned long long uintmax_t;

#define min_intmax_t  (-9223372036854775807LL - 1)
#define max_intmax_t  (9223372036854775807LL)
#define min_uintmax_t (0)
#define max_uintmax_t (18446744073709551615ULL)

// ----- win types -----
typedef void *HANDLE;
typedef void *HWND;
typedef void *HINSTANCE;
typedef u32   DWORD;
typedef i32   BOOL;
