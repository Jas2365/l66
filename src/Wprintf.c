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

static const fmt_handler printf_jump_table[256] = {
    [fmt_integer_i]      =  print_fmt_decimal_d,
    [fmt_decimal_d]      =  print_fmt_decimal_d,
    [fmt_floating_f]     =  print_fmt_floating_f,
    [fmt_exponent_le]    =  print_fmt_exponent_e,
    [fmt_exponent_ue]    =  print_fmt_exponent_e,
    [fmt_generic_lg]     =  print_fmt_generic_g,
    [fmt_generic_ug]     =  print_fmt_generic_g,
    [fmt_hexfloat_la]    =  print_fmt_hexfloat_a,
    [fmt_hexfloat_ua]    =  print_fmt_hexfloat_a,
    [fmt_unsigned_u]     =  print_fmt_unsigned_u,
    [fmt_hexdecimal_lx]  =  print_fmt_hexadecimal_x, 
    [fmt_hexdecimal_ux]  =  print_fmt_hexadecimal_x, 
    [fmt_octal_o]        =  print_fmt_octal_o,
    [fmt_pointer_p]      =  print_fmt_pointer_p,
    [fmt_string_s]       =  print_fmt_string_s,
    [fmt_character_c]    =  print_fmt_character_c,
    [fmt_counter_n]      =  print_fmt_counter_n,
};

i32 printf(const i8* format, ...){
    va_list args;

    buffer32 buf32 = {
        .buffer         = VOID,
        .temp_buffer    = VOID,
        .buffer_idx     = VOID,
        .total_written  = VOID,
    };

    va_start(args, format);


    for(const i8* p = format; *p != char_null_terminator; p++) {
        if(buf32.buffer_idx >= flush_buff_limit) {
            flush_buffer(buf32.buffer, buf32.buffer_idx);
            buf32.total_written += buf32.buffer_idx;
            buf32.buffer_idx = VOID;
        }

        if(*p != char_format){
            buf32.buffer[buf32.buffer_idx++] = *p;               // this writes the normal string
            continue;
        }
        p++; //skip %

        if(*p  == char_format){
            buf32.buffer[buf32.buffer_idx++] = char_format;
            continue;
        }

        // Parser 
        fmtsp32 spec;
        p = parse_fmt(p, &spec, &args);
        
        // format specifiers
        fmt64 fmt_vals = {
            .is_negative    = false,
            .wchar          = VOID,
            .temp_idx       = VOID,
            .wlen           = VOID,
            .max_char       = VOID,
            .max_wchar      = VOID,
            .digit_start    = VOID,
            .written        = VOID,
            .zeros_to_add   = VOID,
            .prec           = VOID,
            .valf           = VOID,
            .val            = VOID,
            .sval           = VOID,
            .absv           = VOID,
            .vallf          = VOID,
            .bytes32        = {},
            .bytes64        = {},
            .aptr           = nullptr,
            .str            = nullptr,
            .wstr           = nullptr,
            .iptr           = nullptr,
            .vptr           = nullptr, 
        };
        
        fmt_handler handler = printf_jump_table[(u8)*p];
        if(handler) {
            handler(&args, &fmt_vals, &buf32, &spec);
        }else {
            default_print(&buf32, &fmt_vals, p);
        }
        copy_to_main_buffer(&buf32, &fmt_vals, &spec);

    }
    return end_printf(&buf32, &args);
}

null print_fmt_decimal_d(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    fmt_vals->sval = (i64)fmt_vals->val;
    if(fmt_vals->sval < VOID) {
        buf32->temp_buffer[fmt_vals->temp_idx++] = char_minus;
        fmt_vals->is_negative = true;
        fmt_vals->val = (u64)-(fmt_vals->sval);
    } else {
        fmt_vals->temp_idx += f_util(buf32->temp_buffer+fmt_vals->temp_idx, spec);
    }
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos((u64)fmt_vals->val, buf32->temp_buffer + fmt_vals->temp_idx, sys_decimal, lower_case);
    fmt_vals->temp_idx += handle_precision(buf32, spec, fmt_vals);
}

null print_floating_util(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    fmt_vals->vallf = f64_get_val(args, spec->len_modifiers);
    if(fmt_vals->vallf <zero_f64) {buf32->temp_buffer[fmt_vals->temp_idx++] = char_minus;
    fmt_vals->vallf = -fmt_vals->vallf; }
    fmt_vals->temp_idx += f_util(buf32->temp_buffer+fmt_vals->temp_idx, spec);
    fmt_vals->prec = (spec->precision >= zero_precision) ? spec->precision : default_precision;
}

null print_fmt_floating_f(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    print_floating_util(args, fmt_vals, buf32, spec);
    fmt_vals->temp_idx += f64tos(fmt_vals->vallf, buf32->temp_buffer+fmt_vals->temp_idx, fmt_vals->prec);
}

null print_fmt_exponent_e(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    print_floating_util(args, fmt_vals, buf32, spec);
    fmt_vals->temp_idx += f64toes(fmt_vals->vallf, buf32->temp_buffer + fmt_vals->temp_idx, fmt_vals->prec, (spec->len_modifiers == char_ue));
}

// should ftos get prec-1 or precision
null print_fmt_generic_g(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    print_floating_util(args, fmt_vals, buf32, spec);
    fmt_vals->absv = (fmt_vals->vallf < zero_f64) ? -fmt_vals->vallf : fmt_vals->vallf;
    if(fmt_vals->absv != zero_f64 && fmt_vals->absv >= absv_high || fmt_vals->absv < absv_low) {
        fmt_vals->temp_idx += f64toes(fmt_vals->vallf, buf32->temp_buffer+fmt_vals->temp_idx, fmt_vals->prec-1, (spec->len_modifiers == char_ug));
    } else fmt_vals->temp_idx += f64tos(fmt_vals->vallf, buf32->temp_buffer + fmt_vals->temp_idx, spec->precision);
}

null print_fmt_hexfloat_a(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
    buf32->temp_buffer[fmt_vals->temp_idx++] = (spec->len_modifiers == char_ux) ? char_ux : char_lx;

    if(spec->len_modifiers == fmt_l){
        fmt_vals->vallf = va_arg( *args, f64);
        memset(fmt_vals->bytes64, VOID, size_64_bytes);
        memcpy(fmt_vals->bytes64, &fmt_vals->vallf, size_64_bytes);
        for(i32 i = f64_start; i >= VOID; i--)
        fmt_vals->temp_idx += itos((u64)(*(fmt_vals->bytes64+i)), buf32->temp_buffer + fmt_vals->temp_idx, sys_hex, (spec->len_modifiers == char_ux));
    } else {
        fmt_vals->valf = (f32)va_arg( *args, f64);
        memset(fmt_vals->bytes32, VOID, size_32_bytes);
        memcpy(fmt_vals->bytes32, &fmt_vals->valf, size_32_bytes);
        for(i32 i = f32_start; i>= VOID; i--)
        fmt_vals->temp_idx += itos((u64)(*(fmt_vals->bytes32+i)), buf32->temp_buffer + fmt_vals->temp_idx, sys_hex, (spec->len_modifiers == char_ux));
    }
}

null print_fmt_unsigned_u(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos(fmt_vals->val, buf32->temp_buffer+fmt_vals->temp_idx, sys_decimal, lower_case);
    fmt_vals->temp_idx += handle_precision(buf32, spec, fmt_vals);
}

null print_fmt_hexadecimal_x(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    if ((spec->flags & fmt_alt) && fmt_vals->val != zero_u64) {
        buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
        buf32->temp_buffer[fmt_vals->temp_idx++] = (spec->len_modifiers == char_ux) ? char_ux : char_lx;
    }
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos(fmt_vals->val, buf32->temp_buffer + fmt_vals->temp_idx, sys_hex, (spec->len_modifiers == char_ux));
    fmt_vals->temp_idx += handle_precision(buf32, spec, fmt_vals);
}
null print_fmt_octal_o(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    
    if((spec->flags & fmt_alt) && fmt_vals->val != zero_u64){
        buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
        // buf32->temp_buffer[temp_idx++] = char_o;                                                Non standard, 0o not recognisabe
    } 
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos(fmt_vals->val, buf32->temp_buffer +fmt_vals->temp_idx , sys_octal, lower_case);
    fmt_vals->temp_idx += handle_precision(buf32, spec, fmt_vals);
    
}

null print_fmt_pointer_p(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec) {
    fmt_vals->vptr = va_arg(*args, null*);
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_lx;
    fmt_vals->temp_idx += itos((u64)fmt_vals->vptr, buf32->temp_buffer + fmt_vals->temp_idx, sys_hex, lower_case);
}

null print_fmt_string_s(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec) {
    if(spec->len_modifiers & fmt_l){
        // wide string 
        fmt_vals->wstr = va_arg(*args, i16*);
        if(!fmt_vals->wstr) fmt_vals->wstr = (const w16*)null_string_w;
        fmt_vals->max_wchar = (spec->precision >= zero_precision) ? spec->precision : max_i32;
        while(fmt_vals->wstr[fmt_vals->wlen] && fmt_vals->wlen < fmt_vals->max_wchar ) fmt_vals->wlen++;

        fmt_vals->written =utf16_to_utf8(fmt_vals->wstr, fmt_vals->wlen, buf32->temp_buffer+fmt_vals->temp_idx, temp_buffer_size -fmt_vals->temp_idx-1);
        fmt_vals->temp_idx += fmt_vals->written;
    } else {
        fmt_vals->str = va_arg(*args, const i8*);
        if(!fmt_vals->str) fmt_vals->str = null_string;
        fmt_vals->max_char = (spec->precision>= zero_precision) ? spec->precision : max_i32;
        while(*fmt_vals->str && (fmt_vals->max_char < zero_precision || fmt_vals->temp_idx < fmt_vals->max_char) && (fmt_vals->temp_idx < temp_buffer_size -1)) {
            buf32->temp_buffer[fmt_vals->temp_idx++] = *fmt_vals->str++;
        }
    }
}

null print_fmt_character_c(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec) {
    fmt_vals->sval = va_arg(*args, i32);

    if(spec->len_modifiers & fmt_l) {
        // wide characters are promoted to integer size in va_args
        fmt_vals->wchar = (w16)fmt_vals->sval;
        fmt_vals->written = utf16_to_utf8(&fmt_vals->wchar, single_char, buf32->temp_buffer+fmt_vals->temp_idx, temp_buffer_size -fmt_vals->temp_idx-1);
        fmt_vals->temp_idx += fmt_vals->written;
    } else {
        buf32->temp_buffer[fmt_vals->temp_idx++] = (i8)fmt_vals->sval; // a char is usually passed as an integer
    }
}

null print_fmt_counter_n(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec)
{
    fmt_vals->iptr = va_arg(*args, i32*);
    if(fmt_vals->iptr){
        *fmt_vals->iptr = buf32->total_written + buf32->buffer_idx;
    }
}

null default_print(buffer32 *buf32, fmt64 *fmt_vals, const i8* p) {
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_format;
    buf32->temp_buffer[fmt_vals->temp_idx++] = *p;
}

i32 end_printf(buffer32 *buf32, va_list* args) {
    flush_buffer(buf32->buffer, buf32->buffer_idx);
    buf32->total_written += buf32->buffer_idx;
    buf32->buffer_idx = VOID;                               // reset the buffer index [ otherwise causes duplication ]
    va_end(*args);
    return buf32->total_written;    
}

null copy_to_main_buffer(buffer32* buf32, fmt64* fmt_vals, fmtsp32* spec) {
    fmt_vals->temp_idx = apply_padding(buf32->temp_buffer, fmt_vals->temp_idx, spec, fmt_vals->is_negative);
    for(i32 i = 0; i< fmt_vals->temp_idx; i++){
        if(buf32->buffer_idx >= flush_buff_limit) {
            flush_buffer(buf32->buffer, buf32->buffer_idx);
            buf32->total_written += buf32->buffer_idx;
            buf32->buffer_idx = VOID;
        }
        buf32->buffer[buf32->buffer_idx++] = buf32->temp_buffer[i];
    }
}