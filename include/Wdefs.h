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
#include "Wtypes.h"

#define VOID         (0)
#define exit_success (0)
#define exit_failure (1)

#define WIN_IMPORT __declspec(dllimport)

#if defined(_WIN32) && !defined(_WIN64)
    #define WINAPI __stdcall
#else
    #define WINAPI
#endif

#define WINBASEAPI WIN_IMPORT

#define INVALID_HANDLE_VALUE ((HANDLE)(uintptr_t)-1)
#define STD_INPUT_HANDLE     ((DWORD)-10)
#define STD_OUTPUT_HANDLE    ((DWORD)-11)

#define stdout GetStdHandle(STD_OUTPUT_HANDLE)
#define stdin  GetStdHandle(STD_INPUT_HANDLE)

typedef i8* va_list;

#ifdef _WIN64
    #define va_align(type) ((sizeof(type) +7 ) & ~7) // x64 aligns stack argumnets to 8 bytes

#elif defined(_WIN32)
    #define va_align(type) ((sizeof(type) +3 ) & ~3) // x86 aligns stack argumnets to 4 bytes

#endif

#define va_start(ap, last) ((null)   (ap = (va_list)&(last) + va_align(last)))
#define va_arg(ap, type)   (*(type*)((ap += va_align(type)) - va_align(type)))
#define va_end(ap)         ((null)(ap = 0))

#define buf_size         2048
#define temp_buffer_size 1024
#define flush_buff_limit  768

#define flag_left  '-'
#define flag_plus  '+'
#define flag_space ' '
#define flag_zero  '0'
#define flag_alt   '#'

#define dyn_width  '*'
#define char_la    'a'
#define char_ua    'A'

#define char_zero  '0'
#define char_one   '1'
#define char_two   '2'
#define char_three '3'
#define char_four  '4'
#define char_five  '5'
#define char_siz   '6'
#define char_seven '7'
#define char_eight '8'
#define char_nine  '9'

#define char_space ' '

#define char_format '%'
#define char_minus  '-'
#define char_plus   '+'
#define char_space  ' '
#define char_lx     'x'
#define char_ux     'X'
#define char_le     'e'
#define char_ue     'E'
#define char_lg     'g'
#define char_ug     'G'
#define char_lp     'p'
#define char_up     'P'
#define char_o      'o'

#define char_null_terminator '\0'

#define decimal_point  '.'
#define is_long        'l'
#define is_sizet       'z'
#define is_shorth      'h'
#define is_charhh      'h'

#define len_h  'h'  // short
#define len_hh 'h'  // char
#define len_l  'l'  // long
#define len_ll 'l'  // longlong
// #define len_L  'L'  // double long                   diabled double long
#define len_z  'z'  // s64 
#define len_j  'j'  // greates width integer
#define len_t  't'  // pointer difference

#define lower_case false
#define upper_case true

#define no_padding        (i32)( 0 )
#define no_precision      (i32)( -1 )
#define default_precision (i32)( 6 ) 
#define absv_high         (f64)( 1000000.0 )
#define absv_low          (f64)( 0.0001 )
#define zero_f64          (f64)( 0.0 )
#define zero_precision    (i32)( 0 )
#define zero_u64          (u64)( 0 )

#define size_32_bytes     (i32)( 4 )
#define size_64_bytes     (i32)( 8 )
#define f32_start         (i32)( size_32_bytes -1 )
#define f64_start         (i32)( size_64_bytes -1 )

#define fmt_integer_i       'i'
#define fmt_decimal_d       'd'
#define fmt_floating_f      'f'
#define fmt_hexdecimal_lx   'x'
#define fmt_hexdecimal_ux   'X'
#define fmt_exponent_le     'e'
#define fmt_exponent_ue     'E'
#define fmt_generic_lg      'g'
#define fmt_generic_ug      'G'
#define fmt_hexfloat_la     'a'
#define fmt_hexfloat_ua     'A'
#define fmt_unsigned_u      'u'
#define fmt_octal_o         'o'
#define fmt_character_c     'c'
#define fmt_string_s        's'
#define fmt_pointer_p       'p'
#define fmt_counter_n       'n'
#define fmt_char_format     '%'

#define null_string      "(null)" 
#define null_string_w    L"(null)"
#define nan_l             "NAN"
#define nan_u             "nan"
#define inf_l             "inf"
#define inf_u             "INF"

#define single_char 1

enum numsys : i32 {
    sys_binary = 2,
    sys_octal = 8,
    sys_decimal = 10,
    sys_hex = 16,
};

enum fmt_flags: i32 {
    fmt_left  = 0x01, // left align
    fmt_plus  = 0x02, // force a plus sign
    fmt_space = 0x04, // spaces
    fmt_zero  = 0x08, //  pad with zeros
    fmt_alt   = 0x10, // '#' alternate 0x 0o 0b
};

enum fmt_modifers: i32 {
    fmt_n  = 0x00, // normal
    fmt_h  = 0x01, // short
    fmt_hh = 0x02, // character
    fmt_l  = 0x04, // long = 32bit on 64 bit
    fmt_ll = 0x08, // long 64
    fmt_z  = 0x10, // sizet
    fmt_j  = 0x20, // greates width integer
    fmt_t  = 0x40, // pointer difference
    // fmt_L  = 0x80, // for floats                     disabled double long
};