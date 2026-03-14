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

#include "Wstring.h"
#include "Wmem.h"

i32 check_nan(i8* buffer, i32 len) {
    buffer[len++] = 'N';
    buffer[len++] = 'a';
    buffer[len++] = 'N';
    return len;
}

i32 check_max(i8* buffer, i32 len) {
    buffer[len++] = 'I';
    buffer[len++] = 'n';
    buffer[len++] = 'f';
    return len;
}

i32 check_min(i8* buffer, i32 len) {
    buffer[len++] = '-';
    buffer[len++] = 'I';
    buffer[len++] = 'n';
    buffer[len++] = 'f';
    return len;
}

i32 itos(u64 val, i8* buffer, i32 base, boolean capitalised) {
    if(val == 0) {
        buffer[0] = '0';
        return 1;
    }

    i32 i = 0;
    if(val != val) return check_nan(buffer, i);

    static i8 temp[itos_temp_size];
     
    const i8* digits = capitalised ? upper_case_digits : lower_case_digits;

    while(val > 0 && i < itos_temp_size -1) {
        temp[i++] = digits[val % base];
        val /= base;
    }

    i32 len = i, j = 0;
    while(i > 0) buffer[j++] = temp[--i];
    return len;
}

i32 f32tos(f64 val, i8* buffer, i32 precision){ // f64 val cus the compiler promotes floats to double
    i32 len = 0;
    if (val < 0) { buffer[len++] = '-'; val = -val; }
    
    if(val != val)    return check_nan(buffer, len);
    if(val > max_f32) return check_max(buffer, len);
    if(val < min_f32) return check_min(buffer, len);

    
    f32 rounding = 0.5;
    for(int i = 0; i<precision; i++) rounding /= 10.0;
    val += rounding;

    u64 ipart = (u64)val;
    f64 fpart = val - (f32)ipart;
    len += itos(ipart, buffer + len, sys_decimal, lower_case);
    
    if(precision > 0) {
        buffer[len++] = '.';
        i32 digit;
        for(i32 i = 0; i< precision; i++) {
            fpart *= 10.0;
            digit = (i32)fpart;
            buffer[len++] = digit + '0';
            fpart -= digit;
        }
    }
    return len;
}
i32 f64tos(f64 val, i8* buffer, i32 precision){
    i32 len = 0;
    if (val < 0) { buffer[len++] = '-'; val = -val; }
    
    if(val != val)    return check_nan(buffer, len);
    if(val > max_f64) return check_max(buffer, len);
    if(val < min_f64) return check_min(buffer, len);

    f64 rounding = 0.5;
    for(int i = 0; i<precision; i++) rounding /= 10.0;
    val += rounding;

    u64 ipart = (u64)val;
    f64 fpart = val - (f64)ipart;
    len += itos(ipart, buffer + len, sys_decimal, lower_case);
    
    if(precision > 0) {
        buffer[len++] = '.';
        i32 digit;
        for(i32 i = 0; i< precision; i++) {
            fpart *= 10.0;
            digit = (i32)fpart;
            buffer[len++] = digit + '0';
            fpart -= digit;
        }
    }
    return len;
}

i32 f32toes(f64 val, i8* buffer, i32 precision, boolean capitalised) {
    i32 len = 0;

    if(val < 0) { buffer[len++] = '-'; val = -val; }
    if(val != val)    return check_nan(buffer, len);
    if(val > max_f32) return check_max(buffer, len);
    if(val < min_f32) return check_min(buffer, len);

    i32 ex = 0;
    if(val > 0.0) {
        // normalise [1,10)
        while(val >= 10.0) { val /= 10.0; ex++; }
        while(val < 1.0) { val *= 10.0; ex--; }
    }
    len += f32tos(val, buffer + len, precision);

    buffer[len++] = capitalised ? 'E' : 'e';
    buffer[len++] = (ex >= 0) ? '+' : '-';

    i32 abs_ex = (ex < 0) ? -ex : ex;
    if(abs_ex < 10) buffer[len++] = '0';
    len+= itos((u64)abs_ex, buffer + len, sys_decimal, capitalised);
    return len;
}
i32 f64toes(f64 val, i8* buffer, i32 precision, boolean capitalised) {
    i32 len = 0;
    if(val < 0) { buffer[len++] = '-'; val = -val; }
    if(val != val)    return check_nan(buffer, len);
    if(val > max_f64) return check_max(buffer, len);
    if(val < min_f64) return check_min(buffer, len);


    i32 ex = 0;
    if(val > 0.0) {
        // normalise [1,10)
        while(val >= 10.0) { val /= 10.0; ex++; }
        while(val < 1.0) { val *= 10.0; ex--; }
    }
    len += f64tos(val, buffer + len, precision);

    buffer[len++] = capitalised ? 'E' : 'e';
    buffer[len++] = (ex >= 0) ? '+' : '-';

    i32 abs_ex = (ex < 0) ? -ex : ex;
    if(abs_ex < 10) buffer[len++] = '0';
    len+= itos((u64)abs_ex, buffer + len, sys_decimal, capitalised);
    return len;
}

i32 utf16_to_utf8(const w16 *wstr, i32 wlen, i8 *buf, i32 max_buf_len) {
    i32 d_idx = 0;

    for(i32 i =0; i < wlen && d_idx <max_buf_len; i++){
        w16 wc = (w16)wstr[i];

        // 1byte ASCii (0x0000 to 0x007f)
        if(wc < 0x0080) {
            if(d_idx + 1 >max_buf_len) break;
            buf[d_idx++] = (i8)wc;
        }
        // 2 byte utf-8 (0x0080 to 0x07ff)
        else if(wc < 0x0800){
            if(d_idx+ 2 > max_buf_len) break;
            buf[d_idx++] = (i8)(0xc0 | (wc >> 6));
            buf[d_idx++] = (i8)(0x80 | (wc & 0x3f)); 
        }
        // 4byte utf-8 Surrogate Pairs (emojis)
        else if (wc > 0xd800 && wc <= 0xdbff) { // high surrogate
            if(i +1 < wlen && d_idx + 4 <= max_buf_len){
                w16 wc2 = (w16)wstr[i+1];
                if(wc2 >= 0xdc00 && wc2 <= 0xdfff) { // low surrogate
                    u32 codepoint = 0x10000 +(((wc & 0x3ff) << 10) | (wc2 &0x3ff));
                    buf[d_idx++]  = (i8)(0xf0 | (codepoint >> 18));
                    buf[d_idx++]  = (i8)(0x80 |((codepoint >> 12) & 0x3f ));
                    buf[d_idx++]  = (i8)(0x80 |((codepoint >> 6) & 0x3f ));
                    buf[d_idx++]  = (i8)(0x80 | (codepoint & 0x3f));
                    i++; // skip low surrogate
                }
            }
        }
        // 3 byte utf-8 (0x0800 to 0xffff)
        else {
            if(d_idx +3 > max_buf_len) break;
            buf[d_idx++] = (i8)(0xe0 | (wc >> 12));
            buf[d_idx++] = (i8)(0x80 | ((wc >> 6) & 0x3f));
            buf[d_idx++] = (i8)(0x80 | (wc & 0x3f));
        }
    }
    return d_idx;
}



// f128 functions have been commented out for future use
// i32 f128tos(f128 val, i8* buffer, i32 precision){
//     i32 len = 0;
//     if (val < 0) { buffer[len++] = '-'; val = -val; }
    
//     if(val != val)    return check_nan(buffer, len);
//     if(val > max_f128) return check_max(buffer, len);
//     if(val < min_f128) return check_min(buffer, len);

    
//     f128 rounding = 0.5;
//     for(int i = 0; i<precision; i++) rounding /= 10.0;
//     val += rounding;

//     u64 ipart = (u64)val;
//     f64 fpart = val - (f128)ipart;
//     len += itos(ipart, buffer + len, sys_decimal, lower_case);
    
//     if(precision > 0) {
//         buffer[len++] = '.';
//         i32 digit;
//         for(i32 i = 0; i< precision; i++) {
//             fpart *= 10.0;
//             digit = (i32)fpart;
//             buffer[len++] = digit + '0';
//             fpart -= digit;
//         }
//     }
//     return len;
// }

// i32 f128toes(f128 val, i8* buffer, i32 precision, boolean capitalised) {
//     i32 len = 0;

//     if(val < 0.0L) { buffer[len++] = '-'; val = -val; }
//     if(val != val)    return check_nan(buffer, len);
//     if(val > max_f128) return check_max(buffer, len);
//     if(val < min_f128) return check_min(buffer, len);

//     i32 ex = 0;
//     if(val > 0.0L) {
//         // normalise [1,10)
//         while(val >= 10.0L) { val /= 10.0L; ex++; }
//         while(val < 1.0L) { val *= 10.0L; ex--; }
//     }
//     len += f128tos(val, buffer + len, precision);

//     buffer[len++] = capitalised ? 'E' : 'e';
//     buffer[len++] = (ex >= 0) ? '+' : '-';

//     i32 abs_ex = (ex < 0) ? -ex : ex;
//     if(abs_ex < 10) buffer[len++] = '0';
//     len+= itos((u64)abs_ex, buffer + len, sys_decimal, capitalised);
//     return len;
// }

// i32 f128tohex(f128 val, i8 *buffer, i32 precision, boolean capitalised) {
//     i32 len = 0;
//     u8 bytes[16];
//     memset(bytes, NULL, sys_hex);
//     memcpy(bytes, &val, sys_hex);
//     // extract bytes based on your raw byte observation
//     // mantissa is bytes 0-7,exponent is bytes 8-9
    
//     u64 mantissa = *(u64*)&bytes[0];
//     u16 exp_bits = *(u16*)&bytes[8];
//     u16 exp_raw = exp_bits & max_i16;
    
//     // 0x
//     buffer[len++] = char_zero;
//     buffer[len++] = capitalised ? char_ux : char_lx;

//     boolean is_msb_set = (mantissa >> 63) & 1; // bit 63 is the integer bit
//     buffer[len++] = is_msb_set ? char_one : char_zero;

//     if(precision != 0){
//         buffer[len++] = decimal_point;
    
//         u64 fraction = mantissa << 1; // discard integer bit
//         i32 hex_digit = (precision >= 0) ? precision : def_hex_precision;

//         for(i32 i = 0 ; i< hex_digit; i++){
//             if(precision < 0 && fraction == 0) { // stop printing ifremaining bits are all zero
//                 if(i == 0) buffer[len++] = char_zero; // atleast print one zero
//                 else break; 
//             }
//             // extract top 4 bits
//             u8 hex_val = (u8)(fraction >> 60);
//             buffer[len++] = (hex_val < 10) ? (hex_val + char_zero) 
//             :(hex_val -10 + (capitalised ? char_ua: char_la));
//             fraction <<= 4;
//         }
//     }

//     // 5 exponent
//     buffer[len++] = capitalised ? char_up : char_lp;

//     i32 true_exp;

//     if(exp_raw == 0){
//         true_exp = (mantissa == 0) ?  0 : exp_bias_neg;
//     } else {
//         true_exp = (i32)exp_raw - exp_bias;
//     }

//     // sign of exp
//     if(true_exp >= 0){
//         buffer[len++] = char_plus;
//     } else {
//         buffer[len++] = char_minus;
//         true_exp = -true_exp;
//     }
//     len += itos((u64)true_exp, buffer  + len, sys_decimal, lower_case);
//     return len;
// }
