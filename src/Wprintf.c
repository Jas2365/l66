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

i32 printf(const i8* format, ...){
    va_list args;
    buffer32 buf32 = {
        .buffer = NULL,
        .temp_buffer = NULL,
        .buffer_idx = 0,
        .total_written = 0
    };

    va_start(args, format);


    for(const i8* p = format; *p != char_null_terminator; p++) {
        if(buf32.buffer_idx >= flush_buff_limit) {
            flush_buffer(buf32.buffer, buf32.buffer_idx);
            buf32.total_written += buf32.buffer_idx;
            buf32.buffer_idx = 0;
        }

        if(*p != char_format){
            buf32.buffer[buf32.buffer_idx++] = *p;
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
            print_fmt_decimal_d(&args, &fmt_vals, &buf32, &spec);
            break;
        case fmt_floating_f:
            print_fmt_floating_f(&args, &fmt_vals, &buf32, &spec);
            break;
        case fmt_exponent_le:
            print_fmt_exponent_e(&args, &fmt_vals, &buf32, &spec, lower_case);
            break;
        case fmt_exponent_ue: 
            print_fmt_exponent_e(&args, &fmt_vals, &buf32, &spec, upper_case);
            break;
        case fmt_generic_lg:
            print_fmt_generic_g(&args, &fmt_vals, &buf32, &spec, lower_case);
            break;
        case fmt_generic_ug: 
            print_fmt_generic_g(&args, &fmt_vals, &buf32, &spec, upper_case);
            break;
        case fmt_hexfloat_la:
        case fmt_hexfloat_ua:
            // need to rewrite this in accordance to SSE2
            
            break;
        case fmt_unsigned_u: 
            print_fmt_unsigned_u(&args, &fmt_vals, &buf32, &spec);
            break;
        case fmt_hexdecimal_lx: 
            print_fmt_hexadecimal_x(&args, &fmt_vals, &buf32, &spec, lower_case);
            break;
        case fmt_hexdecimal_ux: 
            print_fmt_hexadecimal_x(&args, &fmt_vals, &buf32, &spec, upper_case);
            break;
        case fmt_octal_o:   
            print_fmt_octal_o(&args, &fmt_vals, &buf32, &spec);
            break;
        case fmt_pointer_p: 
            print_fmt_pointer_p(&args, &fmt_vals, &buf32);
            break;
        case fmt_string_s:
            print_fmt_string_s(&args, &spec, &fmt_vals, &buf32);
            break;
        case fmt_character_c: 
            print_fmt_character_c(&args, &spec, &fmt_vals, &buf32);
            break;
        case fmt_char_format: 
            print_fmt_char_format(&fmt_vals, &buf32); 
            break;
        case fmt_counter_n:
            print_fmt_counter_n(&args, &fmt_vals, &buf32);
            break;
        default:  
            default_print(&buf32, &fmt_vals, p);
            break;
        }
        // if (temp_idx == 0) { buf32.temp_buffer[temp_idx++] = '!'; }                     // disabled  Debug marker
        // padding
       
        // copy to main buffer
        copy_to_main_buffer(&buf32, &fmt_vals, &spec);
    }
    return end_printf(&buf32, &args);
}

null print_fmt_decimal_d(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {

    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    fmt_vals->sval = (i64)fmt_vals->val;
    if(fmt_vals->sval < 0) {
        buf32->temp_buffer[fmt_vals->temp_idx++] = char_minus;
        fmt_vals->is_negative = true;
        fmt_vals->val = (u64)-(fmt_vals->sval);
    } else {
        if(spec->flags & fmt_plus) buf32->temp_buffer[fmt_vals->temp_idx++] = char_plus;
        else if(spec->flags & fmt_space) buf32->temp_buffer[fmt_vals->temp_idx++] = char_space;
    }
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos((u64)fmt_vals->val, buf32->temp_buffer + fmt_vals->temp_idx, sys_decimal, lower_case);
    fmt_vals->temp_idx += handle_precision(buf32, spec, fmt_vals);
}

null print_fmt_floating_f(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec)
{
    fmt_vals->valld = f64_get_val(args, spec->len_modifiers);
    if(fmt_vals->valld <0.0) {buf32->temp_buffer[fmt_vals->temp_idx++] = char_minus;
    fmt_vals->valld = -fmt_vals->valld; }
    fmt_vals->temp_idx += f_util(buf32->temp_buffer+fmt_vals->temp_idx, spec);
    fmt_vals->prec = (spec->precision >= 0) ?spec->precision :default_precision;
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->temp_idx += f64tos(fmt_vals->valld, buf32->temp_buffer+fmt_vals->temp_idx, fmt_vals->prec);

}

null print_fmt_exponent_e(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec, boolean capitalised) {
    fmt_vals->valld = f64_get_val(args, spec->len_modifiers);
    if(fmt_vals->valld <0.0) {buf32->temp_buffer[fmt_vals->temp_idx++] = char_minus;
    fmt_vals->valld = -fmt_vals->valld; }
    fmt_vals->temp_idx += f_util(buf32->temp_buffer+fmt_vals->temp_idx, spec);
    fmt_vals->prec = (spec->precision >= 0) ? spec->precision : default_precision;
    fmt_vals->temp_idx += f64toes(fmt_vals->valld, buf32->temp_buffer + fmt_vals->temp_idx, fmt_vals->prec, capitalised);
}

null print_fmt_generic_g(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec, boolean capitalised) {
    fmt_vals->valld = f64_get_val(args, spec->len_modifiers);
    if(fmt_vals->valld <0.0) {buf32->temp_buffer[fmt_vals->temp_idx++] = char_minus;
    fmt_vals->valld = -fmt_vals->valld;}
    fmt_vals->temp_idx += f_util(buf32->temp_buffer+fmt_vals->temp_idx, spec);
    fmt_vals->prec = (spec->precision>=0) ? spec->precision : default_precision;
    fmt_vals->absv = (fmt_vals->valld < 0.0) ? -fmt_vals->valld : fmt_vals->valld;
    if(fmt_vals->absv != 0.0 && fmt_vals->absv >= 1000000.0 || fmt_vals->absv < 0.0001) {
        fmt_vals->temp_idx += f64toes(fmt_vals->valld, buf32->temp_buffer+fmt_vals->temp_idx, fmt_vals->prec-1, capitalised);
    } else fmt_vals->temp_idx += f64tos(fmt_vals->valld, buf32->temp_buffer + fmt_vals->temp_idx, spec->precision);
}

null print_fmt_unsigned_u(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos(fmt_vals->val, buf32->temp_buffer+fmt_vals->temp_idx, sys_decimal, lower_case);
    if(spec->precision > fmt_vals->written) {
        fmt_vals->zeros_to_add = spec->precision - fmt_vals->written;
        // shifting
        for(i32 i = fmt_vals->written -1; i>=0; i--)
            buf32->temp_buffer[fmt_vals->digit_start +i +fmt_vals->zeros_to_add] = buf32->temp_buffer[fmt_vals->digit_start+i];
        // fill
        for(i32 i = 0; i< fmt_vals->zeros_to_add; i++)
            buf32->temp_buffer[fmt_vals->digit_start + i] = char_zero;
        // set len
        fmt_vals->temp_idx += spec->precision;
    } else fmt_vals->temp_idx += fmt_vals->written;
}

null print_fmt_hexadecimal_x(va_list *args, fmt64 *fmt_vals, buffer32 *buf32, fmtsp32 *spec, boolean capitalised) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    if ((spec->flags & fmt_alt) && fmt_vals->val != 0) {
        buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
        buf32->temp_buffer[fmt_vals->temp_idx++] = capitalised ? char_ux : char_lx;
    }
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos(fmt_vals->val, buf32->temp_buffer + fmt_vals->temp_idx, sys_hex, capitalised);
    if(spec->precision > fmt_vals->written) {
        fmt_vals->zeros_to_add = spec->precision - fmt_vals->written;
        for(i32 i = fmt_vals->written -1; i>=0; i--) {
            buf32->temp_buffer[fmt_vals->digit_start+i +fmt_vals->zeros_to_add] = buf32->temp_buffer[fmt_vals->digit_start+i];
        }
        for(i32 i = 0; i<fmt_vals->zeros_to_add; i++) {
            buf32->temp_buffer[fmt_vals->digit_start +i] = char_zero;
        }
        fmt_vals->temp_idx += spec->precision;
    } else fmt_vals->temp_idx += fmt_vals->written;
}
null print_fmt_octal_o(va_list* args, fmt64* fmt_vals, buffer32* buf32, fmtsp32* spec) {
    fmt_vals->val = u64_get_val(args, spec->len_modifiers);
    
    if((spec->flags & fmt_alt) && fmt_vals->val != 0){
        buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
        // buf32->temp_buffer[temp_idx++] = char_o;                                                Non standard, 0o not recognisabe
    } 
    fmt_vals->digit_start = fmt_vals->temp_idx;
    fmt_vals->written += itos(fmt_vals->val, buf32->temp_buffer +fmt_vals->temp_idx , sys_octal, lower_case);
    if(spec->precision > fmt_vals->written) {
        fmt_vals->zeros_to_add = spec->precision - fmt_vals->written;
        for(i32 i  = fmt_vals->written -1; i>=0; i--)
            buf32->temp_buffer[fmt_vals->digit_start+i+fmt_vals->zeros_to_add] = buf32->temp_buffer[fmt_vals->digit_start+i];
        for(i32 i = 0; i< fmt_vals->zeros_to_add; i++)
            buf32->temp_buffer[fmt_vals->digit_start+i] = char_zero;
        fmt_vals->temp_idx += spec->precision;
    } else fmt_vals->temp_idx += fmt_vals->written;
    
}

null print_fmt_pointer_p(va_list* args, fmt64* fmt_vals, buffer32* buf32) {
    fmt_vals->vptr = va_arg(*args, null*);
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_zero;
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_lx;
    fmt_vals->temp_idx += itos((u64)fmt_vals->vptr, buf32->temp_buffer + fmt_vals->temp_idx, sys_hex, lower_case);
}

null print_fmt_string_s(va_list* args, fmtsp32* spec, fmt64* fmt_vals, buffer32* buf32) {
    if(spec->len_modifiers & fmt_l){
        fmt_vals->wstr = va_arg(*args, i16*);
        if(!fmt_vals->wstr) fmt_vals->wstr = (const w16*)null_string_w;
        fmt_vals->max_wchar = (spec->precision >= 0) ? spec->precision : max_i32;
        while(fmt_vals->wstr[fmt_vals->wlen] && fmt_vals->wlen < fmt_vals->max_wchar ) fmt_vals->wlen++;

        fmt_vals->written =utf16_to_utf8(fmt_vals->wstr, fmt_vals->wlen, buf32->temp_buffer+fmt_vals->temp_idx, temp_buffer_size -fmt_vals->temp_idx-1);
        fmt_vals->temp_idx += fmt_vals->written;
    }
    else {
        fmt_vals->str = va_arg(*args, const i8*);
        if(!fmt_vals->str) fmt_vals->str = null_string;
        fmt_vals->max_char = (spec->precision>= 0) ? spec->precision : -1;
        while(*fmt_vals->str && (fmt_vals->max_char < 0 || fmt_vals->temp_idx < fmt_vals->max_char) && (fmt_vals->temp_idx < temp_buffer_size -1)) {
            buf32->temp_buffer[fmt_vals->temp_idx++] = *fmt_vals->str++;
        }
    }
}

null print_fmt_character_c(va_list* args, fmtsp32* spec, fmt64* fmt_vals, buffer32* buf32) {
    fmt_vals->sval = va_arg(*args, i32);

    if(spec->len_modifiers & fmt_l) {
        // wide characters are promoted to integer size in va_args
        fmt_vals->wchar = (w16)fmt_vals->sval;
        fmt_vals->written = utf16_to_utf8(&fmt_vals->wchar, single_char, buf32->temp_buffer+fmt_vals->temp_idx, temp_buffer_size -fmt_vals->temp_idx-1);
        fmt_vals->temp_idx += fmt_vals->written;
    } else{
        buf32->temp_buffer[fmt_vals->temp_idx++] = (i8)fmt_vals->sval; // a char is usually passed as an integer
    }
}

null print_fmt_char_format(fmt64 *fmt_vals, buffer32 *buf32) {
    buf32->temp_buffer[fmt_vals->temp_idx++] = char_format;
}
null print_fmt_counter_n(va_list *args, fmt64 *fmt_vals, buffer32 *buf32)
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
    buf32->buffer_idx = 0;                               // reset the buffer index [ otherwise causes duplication ]
    va_end(*args);
    return buf32->total_written;    
}

null copy_to_main_buffer(buffer32* buf32, fmt64* fmt_vals, fmtsp32* spec) {
    fmt_vals->temp_idx = apply_padding(buf32->temp_buffer, fmt_vals->temp_idx, spec, fmt_vals->is_negative);
    for(i32 i = 0; i< fmt_vals->temp_idx; i++){
        if(buf32->buffer_idx >= flush_buff_limit) {
            flush_buffer(buf32->buffer, buf32->buffer_idx);
            buf32->total_written += buf32->buffer_idx;
            buf32->buffer_idx = 0;
        }
        buf32->buffer[buf32->buffer_idx++] = buf32->temp_buffer[i];
    }
}