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

#define maxf32 3.402823e38
#define maxf64 1.797639e308

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
