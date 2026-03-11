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
#include "Wmem.h"
#include "Wstring.h"
#include "Wdefs.h"


static null reset_fmt(fmt_spec_ptr_t spec) {
    spec->flags = fmt_n;
    spec->width = 0;
    spec->precision = -1;
    spec->len_modifiers = fmt_n;
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

static boolean is_modifer(const i8* p) {
    return (
        *p  ==  len_h  ||
        *p  ==  len_hh ||
        *p  ==  len_l  ||
        *p  ==  len_ll ||
        *p  ==  len_L  ||
        *p  ==  len_z  ||
        *p  ==  len_j  ||
        *p  ==  len_t
    );
}

static null flush_buffer(const i8* buffer, i32 len){
    if(len > 0){
        u32 written = 0;
        WriteFile(stdout, buffer, len, &written, NULL);
    }
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
    } else if(*p == len_L) {
        spec->len_modifiers = fmt_L;
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

static u64 u64_get_val(va_list *args, i32 len_mod) {
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

static f128 f128_get_val(va_list *args, i32 len_mod) {
    if(len_mod == fmt_L){
        return va_arg(*args, f128);
    }     
   return (f128)va_arg(*args, f64);
}

static i32 apply_padding(i8* buffer, i32 len, const fmt_spec_ptr_t spec, i32 is_negative) {
    if(len >= spec->width) return len; // no padding

    i32 total_len = spec->width;
    if(total_len > tm_buf_size) {
        total_len = tm_buf_size;
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

static i32 f_util(i8* buf, fmt_spec_ptr_t spec) { 
    i32 len = 0;
    if(spec->flags & fmt_plus) buf[len++] = char_plus;
    else if(spec->flags & fmt_space) buf[len++] = char_space;
    return len;
}
            
i32 printf(const i8* format, ...){
    va_list args;
    va_start(args, format);
    
    i8  buffer[buf_size];
    i8  tm_buf[tm_buf_size];
    i32 b_idx = 0;
    i32 total_written =0;

    for(const i8* p = format; *p != char_null_terminator; p++) {
        if(b_idx >= flush_buff_limit) {
            flush_buffer(buffer, b_idx);
            total_written += b_idx;
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
        u64  val = 0;
        i64  sval = 0;
        f128 absv = 0;
        f128 valld = 0;
        w16 wchar = 0;
        const w16* wstr = NULL;
        i32 wlen = 0;
        i32 max_wchar = 0;
        const i8* str = NULL;
        null* vptr = NULL;
        i32* iptr = NULL;
        i32 temp_len = 0;
        i32 is_negative = 0;
        i32 digit_start = 0;
        i32 written = 0;
        i32 zeros_to_add = 0;
        i32 prec = 0;
        switch (*p) {
        case fmt_integer_i : // %i and %d behave the same in printf
        case fmt_decimal_d :
            val = u64_get_val(&args, spec.len_modifiers);
            sval = (i64)val;
            if(sval < 0) {
                tm_buf[temp_len++] = char_minus;
                is_negative = 1;
                val = (u64)-(sval);
            } else {
                if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
                else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
            }
            digit_start = temp_len;
            written += itos((u64)val, tm_buf + temp_len, sys_decimal, lower_case);
            if(spec.precision > written) {
                zeros_to_add = spec.precision - written;
                // shifting
                for(i32 i = written -1; i>=0; i--){
                    tm_buf[digit_start +i + zeros_to_add] = tm_buf[digit_start+i];
                }
                // fill
                for(i32 i = 0; i <zeros_to_add; i++){
                    tm_buf[digit_start + i] = char_zero;
                }
                temp_len += spec.precision;
            } else temp_len += written;
            break;
        case fmt_floating_f:
            valld = f128_get_val(&args, spec.len_modifiers);
            if(valld < 0.0L) { tm_buf[temp_len++] = char_minus;  valld = -valld; } 
            temp_len += f_util(tm_buf + temp_len, &spec);
            prec = (spec.precision >= 0) ? spec.precision : default_precision;            
            temp_len += f128tos(valld, tm_buf + temp_len, prec);
            break;
        case fmt_exponent_le:
        case fmt_exponent_ue: 
            valld = f128_get_val(&args, spec.len_modifiers);
            if(valld < 0.0L) { tm_buf[temp_len++] = char_minus;  valld = -valld; } 
            temp_len += f_util(tm_buf + temp_len, &spec);
            prec = (spec.precision >= 0) ? spec.precision : default_precision;
            temp_len += f128toes(valld, tm_buf + temp_len, prec, (*p == 'E'));
            break;
        case fmt_generic_lg:
        case fmt_generic_ug: 
            valld = f128_get_val(&args, spec.len_modifiers);
            if(valld < 0.0L) { tm_buf[temp_len++] = char_minus;  valld = -valld; } 
            temp_len += f_util(tm_buf + temp_len, &spec);
            prec = (spec.precision >= 0) ? spec.precision : default_precision;
            absv = (valld < 0.0L) ? -valld : valld;
            if(absv != 0.0 && (absv >= 1000000.0L || absv < 0.0001L)) {
                temp_len += f128toes(valld, tm_buf + temp_len, prec -1 , (*p == 'G'));
            } else temp_len += f128tos(valld, tm_buf +temp_len, prec);
            break;
        case fmt_hexfloat_la:
        case fmt_hexfloat_ua:
            valld = f128_get_val(&args, spec.len_modifiers);
            // u8 degub_bytes[sys_hex];
            // Wmemcpy(degub_bytes, &valld, sys_hex);
            // printf("Raw Bytes of 1.0L: ");
            // for(i32 i = 0; i < 16; i++) {
            //     // Using your hex logic to see the raw memory
            //     printf("%02x ", degub_bytes[i]);
            // }
            // printf("\n");
            if(val < 0.0L) {is_negative =1; valld = -valld; }
            
            if(is_negative ){
                tm_buf[temp_len++] =char_minus;
            } else {
                temp_len += f_util(tm_buf +temp_len, &spec);
            }
            u8 raw_bytes[16];
            Wmemcpy(raw_bytes, &valld, sys_hex);
            u16 exp_raw =(*(u16*)&raw_bytes[8]) & exp_limit;
            u64 mant_raw = (*(u64*)&raw_bytes[0]);

            if(exp_raw == exp_limit){
                if(mant_raw & mantisa_mask) str = (*p == char_ua) ? nan_u:nan_l;
                else str = (*p == char_ua) ? inf_u : inf_l;
                while(*str) tm_buf[temp_len++] = *str++;
            } else{
                temp_len += f128tohex(valld, tm_buf + temp_len, spec.precision, (*p == char_ua));
            }

            break;
        case fmt_unsigned_u: 
            val = u64_get_val(&args, spec.len_modifiers);
            digit_start = temp_len;
            written += itos(val, tm_buf+temp_len, sys_decimal, lower_case);
            if(spec.precision > written) {
                zeros_to_add = spec.precision - written;
                // shifting
                for(i32 i = written -1; i>=0; i--)  
                    tm_buf[digit_start +i + zeros_to_add] = tm_buf[digit_start+i];
                // fill
                for(i32 i = 0; i <zeros_to_add; i++)
                    tm_buf[digit_start + i] = char_zero;
                // set len
                temp_len += spec.precision;
            } else temp_len += written;
            break;
        case fmt_hexdecimal_lx: 
        case fmt_hexdecimal_ux: 
            val = u64_get_val(&args, spec.len_modifiers);
            if ((spec.flags & fmt_alt) && val != 0) {
                tm_buf[temp_len++] = char_zero;
                tm_buf[temp_len++] = (*p == char_ux) ? char_ux : char_lx;
            }
            digit_start = temp_len;
            written += itos(val, tm_buf + temp_len, sys_hex, (*p == 'X'));
            if(spec.precision > written) {
                zeros_to_add = spec.precision - written;
                for(i32 i = written -1; i>=0; i--) {
                    tm_buf[digit_start+i +zeros_to_add] = tm_buf[digit_start+i];
                }
                for(i32 i = 0; i<zeros_to_add; i++) {
                    tm_buf[digit_start +i] = char_zero;
                }
                temp_len += spec.precision;
            } else temp_len += written;
            break;
        case fmt_octal_o:   
            val = u64_get_val(&args, spec.len_modifiers);
            if((spec.flags & fmt_alt) && val != 0){
                tm_buf[temp_len++] = char_zero;
                // tm_buf[temp_len++] = char_o;                                                Non standard, 0o not recognisabe
            } 
            digit_start = temp_len;
            written += itos(val, tm_buf +temp_len , sys_octal, lower_case);
            if(spec.precision > written) {
                zeros_to_add = spec.precision - written;
                for(i32 i  = written -1; i>=0; i--)
                    tm_buf[digit_start+i+zeros_to_add] = tm_buf[digit_start+i];
                for(i32 i = 0; i< zeros_to_add; i++)
                    tm_buf[digit_start+i] = char_zero;
                temp_len += spec.precision;
            } else temp_len += written;
            break;
        case fmt_pointer_p: 
            vptr = va_arg(args, null*);
            tm_buf[temp_len++] = char_zero;
            tm_buf[temp_len++] = char_lx;
            temp_len += itos((u64)vptr, tm_buf + temp_len, sys_hex, lower_case);
            break;
        case fmt_string_s:
            if(spec.len_modifiers & fmt_l){
                wstr = va_arg(args, i16*);
                if(!wstr) wstr = (const w16*)null_string_w;
                max_wchar = (spec.precision >= 0) ? spec.precision : max_i32;
                while(wstr[wlen] && wlen < max_wchar ) wlen++;

                written =utf16_to_utf8(wstr, wlen, tm_buf+temp_len, tm_buf_size -temp_len-1);
                temp_len += written;
            }
            else {
                str = va_arg(args, const i8*);
                if(!str) str = null_string;
                i32 max_chars = (spec.precision>= 0) ? spec.precision : -1;
                while(*str && (max_chars < 0 || temp_len < max_chars) && (temp_len < tm_buf_size -1)) {
                    tm_buf[temp_len++] = *str++;
                }
            }
            break;
        case fmt_character_c: 
            if(spec.len_modifiers & fmt_l) {
                // wide characters are promoted to integer size in va_args
                wchar = (w16)va_arg(args, i32);
                written = utf16_to_utf8(&wchar, single_char, tm_buf+temp_len, tm_buf_size -temp_len-1);
                temp_len += written;
            } else{
                tm_buf[temp_len++] = (i8)va_arg(args, i32); // a char is usually passed as an integer
            }
            break;
        case fmt_char_format: 
            tm_buf[temp_len++] = char_format;
            break;
        case fmt_counter_n:
            iptr = va_arg(args, i32*);
            if(iptr){
                *iptr =total_written + b_idx;
            }
            break;
        default:              
            tm_buf[temp_len++] = char_format;
            tm_buf[temp_len++] = *p;
            break;
        }
        // if (temp_len == 0) { tm_buf[temp_len++] = '!'; }                     // disabled  Debug marker
        // padding
        temp_len = apply_padding(tm_buf, temp_len, &spec, is_negative);
        // copy to main buffer
        for(i32 i = 0; i< temp_len; i++){
            if(b_idx >= flush_buff_limit) {
                flush_buffer(buffer, b_idx);
                total_written += b_idx;
                b_idx = 0;
            }
            buffer[b_idx++] = tm_buf[i];
        }
    }
    flush_buffer(buffer, b_idx);
    total_written += b_idx;
    b_idx = 0;                               // reset the buffer index [ otherwise causes duplication ]
    va_end(args);
    return total_written;
}