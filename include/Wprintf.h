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

#include "Wwin.h"
#include "Wmem.h"
#include "Wstring.h"
#include "Wdefs.h"


static void flush_buffer(const i8* buffer, i32 len){
    if(len > 0){
        u32 written = 0;
        WriteFile(stdout, buffer, len, &written, NULL);
    }
}

static void reset_fmt(fmt_spec_ptr_t spec) {
    spec->flags = 0;
    spec->width = 0;
    spec->precision = -1;
    spec->is_long_long = false;
    spec->is_size_t = false;
}

static boolean is_flag(const i8* p) {
    return (
        *p == flag_left  ||
        *p == flag_plus  ||
        *p == flag_space ||
        *p == flag_zero  ||
        *p == flag_alt 
    );
}


//
// Parse Format Specifier: %[flags][width][.precision][length specifier]
//
static const i8* parse_fmt(const i8* p, fmt_spec_ptr_t spec, va_list args) {
    reset_fmt(spec);

    // flags
    while(*p && is_flag(p)) {
        if      (*p == flag_left  )  spec->flags |= fmt_left;
        else if (*p == flag_plus  )  spec->flags |= fmt_plus;
        else if (*p == flag_space )  spec->flags |= fmt_space;
        else if (*p == flag_zero  )  spec->flags |= fmt_zero;
        else if (*p == flag_alt   )  spec->flags |= fmt_alt;
        p++;
    }

    // width
    if(*p == dyn_width){
        spec->width = va_arg(args, i32);
        p++;
    } else {
        while(*p >= char_zero && *p <= char_nine){
            spec->width = spec->width * 10 + (*p - char_zero);
            p++;
        }
    }

    // Precision
    if(*p == decimal_point) {
        p++;
        spec->precision = 0;
        if(*p == dyn_width) {
            spec->precision = va_arg(args, i32);
            p++;
        } else {
            while(*p >= char_zero && *p <= char_nine) {
                spec->precision = spec->precision * 10 + (*p - char_zero);
                p++;
            }
        }
    }

    // length long long
    if(*p == is_long){
        p++;
        if(*p == is_long) {
            spec->is_long_long = 1;
            p++;
        }
    }

    // length size_t
    if(*p == is_sizet) {
        spec->is_size_t = 1;
        p++;
    }

    return p;
}

static i32 apply_padding(i8* buffer, i32 len, const fmt_spec_ptr_t spec, i32 is_negative) {
    if(len >= spec->width) return len; // no padding

    i32 pad_count = spec->width - len;
    i32 total_len = spec->width;

    // left alignment
    if(spec->flags & fmt_left) {
        for(i32 i = len; i< total_len; i++) buffer[i] = char_space;
        return total_len;
    }

    // right alignment
    i8 pad_char = (spec->flags & fmt_zero) ? char_zero : char_space;

    // sign shifting
    i32 sign_offset = 0;
    if(is_negative || (spec->flags & fmt_plus) || (spec->flags & fmt_space)) {
        sign_offset = 1;
    }

    if(pad_char == char_zero && sign_offset) {
        // move sign to the fromt -123 becomes -0000123;
        for(i32 i = len -1; i>= sign_offset; i--) {
            buffer[i + pad_count] = buffer[i];
        }
        for(i32 i = sign_offset; i< sign_offset + pad_count; i++) {
            buffer[i] = char_zero;
        }
    } else {
        for(i32 i = len -1; i>=0; i--){
            buffer[i + pad_count] = buffer[i];
        }
        for(i32 i = 0; i< pad_count; i++) {
            buffer[i] = pad_char;
        }
    }
    return total_len;
}

#define buf_size 4096
#define tm_buf_size 1024
#define flush_buff_limit 1024

void printf(const i8* format, ...){
    va_list args;
    va_start(args, format);

    static i8  buffer[buf_size];
    static i8  tm_buf[tm_buf_size];
    static i32 b_idx = 0;

    for(const i8* p = format; *p != char_null_terminator; p++) {
        if(b_idx >= flush_buff_limit) {
            flush_buffer(buffer, b_idx);
            b_idx = 0;
        }

        if(*p != char_format){
            buffer[b_idx++] = *p;
            continue;
        }
        p++; //skip %

        if(*p  == char_format){
            buffer[b_idx++] = char_format;
            continue;
        }

        // Parser 
        fmt_spec_t spec;
        p = parse_fmt(p, &spec, args);
        // p--;                                                      // ! could be a bug [ bug ]

        // format specifiers
        u64 val;
        f64 valf;
        i32 temp_len = 0;
        i32 is_negative = 0;
        i32 prec;
        switch (*p) {
        case fmt_decimal_d :
        case fmt_integer_i : 
            if(spec.is_size_t) val = va_arg(args, i64);
            else val = spec.is_long_long ? va_arg(args, i64) : va_arg(args, i32);
            if((i64)val < 0) {
                tm_buf[temp_len++] = char_minus;
                is_negative = 1;
                val = (i64) val == -val ? val : -val;
                temp_len += itos((u64)val, tm_buf + temp_len ,sys_decimal,lower_case);
            } else {
                if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
                else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
                temp_len += itos((u64)val, tm_buf + temp_len, sys_decimal, lower_case);
            }
            break;
        case fmt_floating_f:
            valf = va_arg(args, f64);
            if(valf < 0) tm_buf[temp_len++] = char_minus;
            else if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
            else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
            prec = (spec.precision >= 0) ? spec.precision : default_precision; 
            temp_len += ftos(valf, tm_buf + temp_len, prec);
            break;
        case fmt_exponent_le:
        case fmt_exponent_ue: 
            valf = va_arg(args, f64);
            if(valf < 0) tm_buf[temp_len++] = char_minus;
            else if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
            else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
            prec = (spec.precision >= 0) ? spec.precision : default_precision;
            temp_len += ftoes(valf, tm_buf + temp_len, prec, (*p == 'E'));
            break;
        case fmt_generic_lg:
        case fmt_generic_ug: 
            valf = va_arg(args, f64);
            if(val < 0) tm_buf[temp_len++] = char_minus;
            else if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
            else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
            f64 absv = (valf < 0) ? -valf : valf;
            prec = (spec.precision >= 0) ? spec.precision : default_precision;
            if(absv != 0.0 && (absv >= 1000000.0 || absv < 0.0001)) {
                temp_len += ftoes(valf, tm_buf + temp_len, prec -1 , (*p == 'G'));
            } else {
                temp_len += ftos(valf, tm_buf +temp_len, prec);
            }
            break;
        case fmt_unsigned_u: 
            if(spec.is_size_t) val = va_arg(args, u64);
            else val = spec.is_long_long ? va_arg(args, u64) : va_arg(args, u32);
            temp_len += itos(val, tm_buf, sys_decimal, lower_case);
            break;
        case fmt_hexdecimal_lx: 
        case fmt_hexdecimal_ux: 
            if(spec.is_size_t) val = va_arg(args, u64);
            else val = spec.is_long_long ? va_arg(args, u64) : va_arg(args, u32);
            if ((spec.flags & fmt_alt) && val != 0) {
                tm_buf[temp_len++] = char_zero;
                tm_buf[temp_len++] = (*p == char_ux) ? char_ux : char_lx;
                temp_len += itos(val, tm_buf + temp_len, 16, (*p == 'X'));
            } else {
                temp_len += itos(val, tm_buf , 16, (*p == 'X'));
            }
            break;
        case fmt_octal_o: 
            if(spec.is_size_t) val = va_arg(args, u64);
            else val = spec.is_long_long ? va_arg(args, u64) : va_arg(args, u32);
            if((spec.flags & fmt_alt) && val != 0){
                tm_buf[temp_len++] = char_zero;
                tm_buf[temp_len++] = char_o;
                temp_len += itos(val, tm_buf +temp_len , sys_octal, lower_case);
            } else {
                temp_len += itos(val, tm_buf, 8, 0);
            }
            break;
        case fmt_pointer_p: 
            void* ptr = va_arg(args, void*);
            tm_buf[temp_len++] = char_zero;
            tm_buf[temp_len++] = char_lx;
            temp_len += itos((u64)ptr, tm_buf + temp_len, sys_hex, lower_case);
            break;
        case fmt_string_s: 
            const i8* s = va_arg(args, const i8*);
            if(!s) s = null_string;
            i32 max_chars = (spec.precision>= 0) ? spec.precision : -1;
            while(*s && (max_chars < 0 || temp_len < max_chars)) {
                tm_buf[temp_len++] = *s++;
            }
            break;
        case fmt_character_c: 
            tm_buf[temp_len++] = (i8)va_arg(args, i32);
            break;
        case fmt_char_format: 
            tm_buf[temp_len++] = char_format;
            break;
        default:              
            tm_buf[temp_len++] = char_format;                
            tm_buf[temp_len++] = *p;                
            break;
        }

        // padding
        temp_len = apply_padding(tm_buf, temp_len, &spec, is_negative);

        // copy to main buffer
        for(i32 i = 0; i< temp_len; i++){
            if(b_idx >= flush_buff_limit) {
                flush_buffer(buffer, b_idx);
                b_idx = 0;
            }
            buffer[b_idx++] = tm_buf[i];
        }
    }

    flush_buffer(buffer, b_idx);
    b_idx = 0;   // reset the buffer index [ otherwise causes duplication ]
    va_end(args);
}