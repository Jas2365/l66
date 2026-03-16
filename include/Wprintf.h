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

#include "Wmem.h"

//
// The GNU gcc compiler does the homing
//
i32 printf(const i8* format, ...);

i32 end_printf(buffer32 *buf32, va_list* args);
null copy_to_main_buffer(buffer32* buf32, fmt64* fmt_vals, fmtsp32* spec);

typedef null (*fmt_handler)(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);

null print_fmt_decimal_d     (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_floating_f    (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_exponent_e    (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_generic_g     (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_hexfloat_a    (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_unsigned_u    (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_hexadecimal_x (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_octal_o       (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_pointer_p     (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_string_s      (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_character_c   (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null print_fmt_counter_n     (va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec);
null default_print           (buffer32* arg, fmt64* fmt_val, const i8* p);