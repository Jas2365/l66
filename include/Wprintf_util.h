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
#include "Wdefs.h"

typedef struct fmt_spec {
    i32 flags;
    i32 width;
    i32 precision;
    i32 len_modifiers;
} fmtsp32;
typedef struct Buffer_Struct {
    i8 buffer[buf_size];
    i8 temp_buffer[temp_buffer_size];
    i32 buffer_idx;
    i32 total_written;
} buffer32;

typedef struct format_values {
  boolean   is_negative;
      w16   wchar;
      i32   temp_idx;
      i32   wlen;
      i32   max_char;
      i32   max_wchar;
      i32   digit_start;
      i32   written;
      i32   zeros_to_add;
      i32   prec;
      u64   val;
      i64   sval;
      f64   absv;
      f64   valld;
const i8*   str;
const w16*  wstr;
      i32*  iptr;
      null* vptr;
}fmt64;
null flush_buffer(const i8* buffer, i32 len);


boolean is_flag(const i8* p);
boolean is_modifer(const i8* p);


//
// Parse Format Specifier: %[flags][width][.precision][length specifier]
//
const i8* parse_fmt(const i8* p, fmtsp32* spec, va_list* args);
null reset_fmt(fmtsp32* spec);

u64 u64_get_val(va_list *args, i32 len_mod);
f64 f64_get_val(va_list *args, i32 len_mod);
i32 handle_precision(buffer32* buf32, const fmtsp32* spec, fmt64* fmt_vals);
i32 apply_padding(i8* buffer, i32 len, const fmtsp32* spec, i32 is_negative);

i32 f_util(i8* buf, fmtsp32* spec);
