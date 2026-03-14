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

#include "Wprintf_util.h"
#include "Wwin.h"

null reset_fmt(fmtsp32* spec) {
    spec->flags = fmt_n;
    spec->width = 0;
    spec->precision = -1;
    spec->len_modifiers = fmt_n;
}

boolean is_flag(const i8* p) {
    return (
        *p == flag_left  ||
        *p == flag_plus  ||
        *p == flag_space ||
        *p == flag_zero  ||
        *p == flag_alt 
    );
}

boolean is_modifer(const i8* p) {
    return (
        *p  ==  len_h  ||
        *p  ==  len_hh ||
        *p  ==  len_l  ||
        *p  ==  len_ll ||
        *p  ==  len_z  ||
        *p  ==  len_j  ||
        *p  ==  len_t
    );
}

null flush_buffer(const i8* buffer, i32 len){
    if(len > 0){
        u32 written = 0;
        WriteFile(stdout, buffer, len, &written, NULL);
    }
}

//
// Parse Format Specifier: %[flags][width][.precision][length specifier]
//
const i8* parse_fmt(const i8* p, fmtsp32* spec, va_list* args) {
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
        spec->width = va_arg(*args, i32);
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
            spec->precision = va_arg(*args, i32);
            p++;
        } else {
            while(*p >= char_zero && *p <= char_nine) {
                spec->precision = spec->precision * 10 + (*p - char_zero);
                p++;
            }
        }
    }


    if(*p == len_l){
        spec->len_modifiers = fmt_l;
        p++;
        if(*p == len_ll) { 
            spec->len_modifiers = fmt_ll;
            p++;
        }
    } else if(*p == len_h) {
        spec->len_modifiers = fmt_h;
        p++;
        if(*p == len_hh) {
            spec->len_modifiers = fmt_hh;
            p++;
        }
    } else if(*p == len_z) {
        spec->len_modifiers = fmt_z;
        p++;
    } else if(*p == len_j) {
        spec->len_modifiers = fmt_j;
        p++;
    } else if(*p == len_t) {
        spec->len_modifiers = fmt_t;
        p++;
    } else {
        spec->len_modifiers = fmt_n;
    }
    
    return p;
}

u64 u64_get_val(va_list *args, i32 len_mod) {
    switch (len_mod) {
        case fmt_j  : return        va_arg(*args, u64);
        case fmt_t  : return        va_arg(*args, i64);
        case fmt_z  : return        va_arg(*args, u64);
        case fmt_l  : return        va_arg(*args, i32);
        case fmt_ll : return        va_arg(*args, i64);
        case fmt_h  : return (i16)  va_arg(*args, i32);
        case fmt_hh : return (i8)   va_arg(*args, i32);
        default     : return        va_arg(*args, i32);
    }
}

f64 f64_get_val(va_list *args, i32 len_mod) {
    // the caller promoted float to a double     
   return va_arg(*args, f64);
}

i32 handle_precision(buffer32* buf32, const fmtsp32* spec, fmt64* fmt_vals) {
    if(spec->precision > fmt_vals->written) {
        fmt_vals->zeros_to_add = spec->precision - fmt_vals->written;
        // shifting
        for(i32 i = fmt_vals->written -1; i>=0; i--){
            buf32->temp_buffer[fmt_vals->digit_start +i +fmt_vals->zeros_to_add] = buf32->temp_buffer[fmt_vals->digit_start +i];
        }   
        // fill
        for(i32 i = 0; i<fmt_vals->zeros_to_add; i++){
            buf32->temp_buffer[fmt_vals->digit_start +i] = char_zero;
        }
        return spec->precision;
    } else { 
        return fmt_vals->written;
    }
}

i32 apply_padding(i8* buffer, i32 len, const fmtsp32* spec, i32 is_negative) {
    if(len >= spec->width) return len; // no padding

    i32 total_len = spec->width;
    if(total_len > temp_buffer_size) {
        total_len = temp_buffer_size;
    }
    i32 pad_count = total_len - len;

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

i32 f_util(i8* buf, fmtsp32* spec) { 
    i32 len = 0;
    if(spec->flags & fmt_plus) buf[len++] = char_plus;
    else if(spec->flags & fmt_space) buf[len++] = char_space;
    return len;
}
