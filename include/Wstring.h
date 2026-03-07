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

#define upper_case_digits "0123456789ABCDEF"
#define lower_case_digits "0123456789abcdef"

#define itos_temp_size 65

i32 itos(i64 val, i8* buffer, i32 base, boolean uppercase) {
    
    if(val == 0) {
        buffer[0] = '0';
        return 1;
    }
    
    static i8 temp[itos_temp_size];
    i32 i = 0;
     
    const i8* digits = uppercase ? upper_case_digits : lower_case_digits;

    while(val > 0 && i < itos_temp_size -1) {
        temp[i++] = digits[val % base];
        val /= base;
    }

    i32 len = i, j = 0;
    while(i > 0) buffer[j++] = temp[--i];
    return len;
}

i32 ftos(f64 val, i8* buffer, i32 precision){
    i32 len = 0;
    if (val < 0) { buffer[len++] = '-'; val = -val; }

    if(val != val) {
        buffer[len++] ='N';
        buffer[len++] ='a';
        buffer[len++] ='N';
        return len;
    }

    if(val > maxf64) {
        buffer[len++] ='I';
        buffer[len++] ='n';
        buffer[len++] ='f';
        return len;
    }

    f64 rounding = 0.5;
    for(int i = 0; i<precision; i++) {
        rounding /= 10.0;
    }
    val += rounding;

    u64 ipart = (u64)val;
    f64 fpart = val - (f64)ipart;

    len += itos(ipart, buffer + len, sys_decimal, lower_case);

    if(precision > 0) {
        buffer[len++] = '.';

        for(i32 i = 0; i< precision; i++) {
            fpart *= 10.0;
            i32 digit = (i32)fpart;
            buffer[len++] = digit + '0';
            fpart -= digit;
        }
    }
    return len;
}

i32 ftoes(f64 val, i8* buffer, i32 precision, boolean uppercase) {
    i32 len = 0;

    if(val < 0) { buffer[len++] = '-'; val = -val; }

    i32 ex = 0;
    if(val > 0.0) {
        // normalise [1,10)
        while(val >= 10.0) { val /= 10.0; ex++; }
        while(val < 1.0) { val *= 10.0; ex--; }
    }
    len += ftos(val, buffer + len, precision);

    buffer[len++] = uppercase ? 'E' : 'e';
    buffer[len++] = (ex >= 0) ? '+' : '-';

    i32 abs_ex = (ex < 0) ? -ex : ex;
    if(abs_ex < 10) buffer[len++] = '0';
    len+= itos((u64)abs_ex, buffer + len, sys_decimal, uppercase);
    return len;
}
