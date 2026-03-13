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

#include "Wtypes.h"
#include "Wdefs.h"
#include "Wprintf_util.h"
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

//
// The GNU gcc compiler does the homing
//
i32 printf(const i8* format, ...);

i32 end_printf(buffer32 *buf32, va_list* args);
null copy_to_main_buffer(buffer32* buf32, fmt64* fmt_vals, fmtsp32* spec);

null print_fmt_generic_g(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec, boolean capitalised);
null print_fmt_unsigned_u(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_hexadecimal_x(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec, boolean capitalised);
null print_fmt_octal_o(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_pointer_p(va_list* args, fmt64* fmt_vals, buffer32* buf32);
null print_fmt_string_s(va_list* args, fmtsp32* spec, fmt64* fmt_vals, buffer32* buf32);
null print_fmt_character_c(va_list* args, fmtsp32* spec, fmt64* fmt_vals, buffer32* buf32);
null print_fmt_char_format(fmt64* fmt_vals, buffer32* buf32);
null print_fmt_counter_n(va_list* args, fmt64* fmt_vals, buffer32* buf32);
null default_print(buffer32* arg, fmt64* fmt_val, const i8* p);