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

#include "Wprintf.h"
#include "Wwin.h"
#include "Wstring.h"
#include "Wdefs.h"

#include "Wprintf_util.h"

typedef struct arguments {
    va_list args;
    i8 buffer[buf_size];
    i8 temp_buffer[temp_buffer_size];
    i32 buffer_idx;
    i32 total_written;
} args32;

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

i32 printf(const i8* format, ...){
    
    args32 arg = {
        .args = nullptr,
        .buffer = NULL,
        .temp_buffer = NULL,
        .buffer_idx = 0,
        .total_written = 0
    };

    va_start(arg.args, format);


    for(const i8* p = format; *p != char_null_terminator; p++) {
        if(arg.buffer_idx >= flush_buff_limit) {
            flush_buffer(arg.buffer, arg.buffer_idx);
            arg.total_written += arg.buffer_idx;
            arg.buffer_idx = 0;
        }

        if(*p != char_format){
            arg.buffer[arg.buffer_idx++] = *p;
            continue;
        }
        p++; //skip %

        if(*p  == char_format){
            arg.buffer[arg.buffer_idx++] = char_format;
            continue;
        }

        // Parser 
        fmt_spec_t spec;
        p = parse_fmt(p, &spec, arg.args);
        // p--;                                                      // ! could be a bug [ bug ]

        // format specifiers
        fmt64 fmt_vals = {
            .is_negative    = false,
            .wchar          = 0,
            .temp_idx       = 0,
            .wlen           = 0,
            .max_char       = 0,
            .max_wchar      = 0,
            .digit_start    = 0,
            .written        = 0,
            .zeros_to_add   = 0,
            .prec           = 0,
            .val            = 0,
            .sval           = 0,
            .absv           = 0,
            .valld          = 0,
            .str            = nullptr,
            .wstr           = nullptr,
            .iptr           = nullptr,
            .vptr           = nullptr, 
        };
        
        switch (*p) {
        case fmt_integer_i : // %i and %d behave the same in printf
        case fmt_decimal_d :
            fmt_vals.val = u64_get_val(&arg.args, spec.len_modifiers);
            fmt_vals.sval = (i64)fmt_vals.val;
            if(fmt_vals.sval < 0) {
                arg.temp_buffer[fmt_vals.temp_idx++] = char_minus;
                fmt_vals.is_negative = true;
                fmt_vals.val = (u64)-(fmt_vals.sval);
            } else {
                if(spec.flags & fmt_plus) arg.temp_buffer[fmt_vals.temp_idx++] = char_plus;
                else if(spec.flags & fmt_space) arg.temp_buffer[fmt_vals.temp_idx++] = char_space;
            }
            fmt_vals.digit_start = fmt_vals.temp_idx;
            fmt_vals.written += itos((u64)fmt_vals.val, arg.temp_buffer + fmt_vals.temp_idx, sys_decimal, lower_case);
            if(spec.precision > fmt_vals.written) {
                fmt_vals.zeros_to_add = spec.precision - fmt_vals.written;
                // shifting
                for(i32 i = fmt_vals.written -1; i>=0; i--){
                    arg.temp_buffer[fmt_vals.digit_start +i + fmt_vals.zeros_to_add] = arg.temp_buffer[fmt_vals.digit_start+i];
                }
                // fill
                for(i32 i = 0; i <fmt_vals.zeros_to_add; i++){
                    arg.temp_buffer[fmt_vals.digit_start + i] = char_zero;
                }
                fmt_vals.temp_idx += spec.precision;
            } else fmt_vals.temp_idx += fmt_vals.written;
            break;
        case fmt_floating_f:
            fmt_vals.valld = f64_get_val(&arg.args, spec.len_modifiers);
            if(fmt_vals.valld < 0.0L) { arg.temp_buffer[fmt_vals.temp_idx++] = char_minus;  fmt_vals.valld = -fmt_vals.valld; } 
            fmt_vals.temp_idx += f_util(arg.temp_buffer + fmt_vals.temp_idx, &spec);
            fmt_vals.prec = (spec.precision >= 0) ? spec.precision : default_precision;            
            fmt_vals.temp_idx += f64tos(fmt_vals.valld, arg.temp_buffer + fmt_vals.temp_idx, fmt_vals.prec);
            break;
        case fmt_exponent_le:
        case fmt_exponent_ue: 
            fmt_vals.valld = f64_get_val(&arg.args, spec.len_modifiers);
            if(fmt_vals.valld < 0.0L) { arg.temp_buffer[fmt_vals.temp_idx++] = char_minus;  fmt_vals.valld = -fmt_vals.valld; } 
            fmt_vals.temp_idx += f_util(arg.temp_buffer + fmt_vals.temp_idx, &spec);
            fmt_vals.prec = (spec.precision >= 0) ? spec.precision : default_precision;
            fmt_vals.temp_idx += f64toes(fmt_vals.valld, arg.temp_buffer + fmt_vals.temp_idx, fmt_vals.prec, (*p == 'E'));
            break;
        case fmt_generic_lg:
        case fmt_generic_ug: 
            fmt_vals.valld = f64_get_val(&arg.args, spec.len_modifiers);
            if(fmt_vals.valld < 0.0L) { arg.temp_buffer[fmt_vals.temp_idx++] = char_minus;  fmt_vals.valld = -fmt_vals.valld; } 
            fmt_vals.temp_idx += f_util(arg.temp_buffer + fmt_vals.temp_idx, &spec);
            fmt_vals.prec = (spec.precision >= 0) ? spec.precision : default_precision;
            fmt_vals.absv = (fmt_vals.valld < 0.0L) ? -fmt_vals.valld : fmt_vals.valld;
            if(fmt_vals.absv != 0.0 && (fmt_vals.absv >= 1000000.0L || fmt_vals.absv < 0.0001L)) {
                fmt_vals.temp_idx += f64toes(fmt_vals.valld, arg.temp_buffer + fmt_vals.temp_idx, fmt_vals.prec -1 , (*p == 'G'));
            } else fmt_vals.temp_idx += f64tos(fmt_vals.valld, arg.temp_buffer +fmt_vals.temp_idx, fmt_vals.prec);
            break;
        case fmt_hexfloat_la:
        case fmt_hexfloat_ua:
            // need to rewrite this in accordance to SSE2
            
            break;
        case fmt_unsigned_u: 
            fmt_vals.val = u64_get_val(&arg.args, spec.len_modifiers);
            fmt_vals.digit_start = fmt_vals.temp_idx;
            fmt_vals.written += itos(fmt_vals.val, arg.temp_buffer+fmt_vals.temp_idx, sys_decimal, lower_case);
            if(spec.precision > fmt_vals.written) {
                fmt_vals.zeros_to_add = spec.precision - fmt_vals.written;
                // shifting
                for(i32 i = fmt_vals.written -1; i>=0; i--)  
                    arg.temp_buffer[fmt_vals.digit_start +i + fmt_vals.zeros_to_add] = arg.temp_buffer[fmt_vals.digit_start+i];
                // fill
                for(i32 i = 0; i <fmt_vals.zeros_to_add; i++)
                    arg.temp_buffer[fmt_vals.digit_start + i] = char_zero;
                // set len
                fmt_vals.temp_idx += spec.precision;
            } else fmt_vals.temp_idx += fmt_vals.written;
            break;
        case fmt_hexdecimal_lx: 
        case fmt_hexdecimal_ux: 
            fmt_vals.val = u64_get_val(&arg.args, spec.len_modifiers);
            if ((spec.flags & fmt_alt) && fmt_vals.val != 0) {
                arg.temp_buffer[fmt_vals.temp_idx++] = char_zero;
                arg.temp_buffer[fmt_vals.temp_idx++] = (*p == char_ux) ? char_ux : char_lx;
            }
            fmt_vals.digit_start = fmt_vals.temp_idx;
            fmt_vals.written += itos(fmt_vals.val, arg.temp_buffer + fmt_vals.temp_idx, sys_hex, (*p == 'X'));
            if(spec.precision > fmt_vals.written) {
                fmt_vals.zeros_to_add = spec.precision - fmt_vals.written;
                for(i32 i = fmt_vals.written -1; i>=0; i--) {
                    arg.temp_buffer[fmt_vals.digit_start+i +fmt_vals.zeros_to_add] = arg.temp_buffer[fmt_vals.digit_start+i];
                }
                for(i32 i = 0; i<fmt_vals.zeros_to_add; i++) {
                    arg.temp_buffer[fmt_vals.digit_start +i] = char_zero;
                }
                fmt_vals.temp_idx += spec.precision;
            } else fmt_vals.temp_idx += fmt_vals.written;
            break;
        case fmt_octal_o:   
            fmt_vals.val = u64_get_val(&arg.args, spec.len_modifiers);
            if((spec.flags & fmt_alt) && fmt_vals.val != 0){
                arg.temp_buffer[fmt_vals.temp_idx++] = char_zero;
                // arg.temp_buffer[temp_idx++] = char_o;                                                Non standard, 0o not recognisabe
            } 
            fmt_vals.digit_start = fmt_vals.temp_idx;
            fmt_vals.written += itos(fmt_vals.val, arg.temp_buffer +fmt_vals.temp_idx , sys_octal, lower_case);
            if(spec.precision > fmt_vals.written) {
                fmt_vals.zeros_to_add = spec.precision - fmt_vals.written;
                for(i32 i  = fmt_vals.written -1; i>=0; i--)
                    arg.temp_buffer[fmt_vals.digit_start+i+fmt_vals.zeros_to_add] = arg.temp_buffer[fmt_vals.digit_start+i];
                for(i32 i = 0; i< fmt_vals.zeros_to_add; i++)
                    arg.temp_buffer[fmt_vals.digit_start+i] = char_zero;
                fmt_vals.temp_idx += spec.precision;
            } else fmt_vals.temp_idx += fmt_vals.written;
            break;
        case fmt_pointer_p: 
            fmt_vals.vptr = va_arg(arg.args, null*);
            arg.temp_buffer[fmt_vals.temp_idx++] = char_zero;
            arg.temp_buffer[fmt_vals.temp_idx++] = char_lx;
            fmt_vals.temp_idx += itos((u64)fmt_vals.vptr, arg.temp_buffer + fmt_vals.temp_idx, sys_hex, lower_case);
            break;
        case fmt_string_s:
            if(spec.len_modifiers & fmt_l){
                fmt_vals.wstr = va_arg(arg.args, i16*);
                if(!fmt_vals.wstr) fmt_vals.wstr = (const w16*)null_string_w;
                fmt_vals.max_wchar = (spec.precision >= 0) ? spec.precision : max_i32;
                while(fmt_vals.wstr[fmt_vals.wlen] && fmt_vals.wlen < fmt_vals.max_wchar ) fmt_vals.wlen++;

                fmt_vals.written =utf16_to_utf8(fmt_vals.wstr, fmt_vals.wlen, arg.temp_buffer+fmt_vals.temp_idx, temp_buffer_size -fmt_vals.temp_idx-1);
                fmt_vals.temp_idx += fmt_vals.written;
            }
            else {
                fmt_vals.str = va_arg(arg.args, const i8*);
                if(!fmt_vals.str) fmt_vals.str = null_string;
                fmt_vals.max_char = (spec.precision>= 0) ? spec.precision : -1;
                while(*fmt_vals.str && (fmt_vals.max_char < 0 || fmt_vals.temp_idx < fmt_vals.max_char) && (fmt_vals.temp_idx < temp_buffer_size -1)) {
                    arg.temp_buffer[fmt_vals.temp_idx++] = *fmt_vals.str++;
                }
            }
            break;
        case fmt_character_c: 
            if(spec.len_modifiers & fmt_l) {
                // wide characters are promoted to integer size in va_args
                fmt_vals.wchar = (w16)va_arg(arg.args, i32);
                fmt_vals.written = utf16_to_utf8(&fmt_vals.wchar, single_char, arg.temp_buffer+fmt_vals.temp_idx, temp_buffer_size -fmt_vals.temp_idx-1);
                fmt_vals.temp_idx += fmt_vals.written;
            } else{
                arg.temp_buffer[fmt_vals.temp_idx++] = (i8)va_arg(arg.args, i32); // a char is usually passed as an integer
            }
            break;
        case fmt_char_format: 
            arg.temp_buffer[fmt_vals.temp_idx++] = char_format;
            break;
        case fmt_counter_n:
            fmt_vals.iptr = va_arg(arg.args, i32*);
            if(fmt_vals.iptr){
                *fmt_vals.iptr =arg.total_written + arg.buffer_idx;
            }
            break;
        default:              
            arg.temp_buffer[fmt_vals.temp_idx++] = char_format;
            arg.temp_buffer[fmt_vals.temp_idx++] = *p;
            break;
        }
        // if (temp_idx == 0) { arg.temp_buffer[temp_idx++] = '!'; }                     // disabled  Debug marker
        // padding
        fmt_vals.temp_idx = apply_padding(arg.temp_buffer, fmt_vals.temp_idx, &spec, fmt_vals.is_negative);
        // copy to main buffer
        for(i32 i = 0; i< fmt_vals.temp_idx; i++){
            if(arg.buffer_idx >= flush_buff_limit) {
                flush_buffer(arg.buffer, arg.buffer_idx);
                arg.total_written += arg.buffer_idx;
                arg.buffer_idx = 0;
            }
            arg.buffer[arg.buffer_idx++] = arg.temp_buffer[i];
        }
    }
    flush_buffer(arg.buffer, arg.buffer_idx);
    arg.total_written += arg.buffer_idx;
    arg.buffer_idx = 0;                               // reset the buffer index [ otherwise causes duplication ]
    va_end(arg.args);
    return arg.total_written;
}