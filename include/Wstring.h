#pragma once

#include "Wtypes.h"
#include "Wmem.h"

i32 itos(i64 val, i8* buffer, i32 base, i32 uppercase) {
    
    if(val == 0) {
        buffer[0] = '0';
        return 1;
    }
    
    i8* temp = (i8*)malloc(sizeof(i8) *65);
    i32 i = 0;
    // i32 i = 0, j = 0;
    // while( val > 0) {
    //     u64 rem = val % base;
    //     if(rem < 10) {
    //         temp[i++] = (i8)rem + '0';
    //     } else {
    //         temp[i++] = (i8)rem - 10 + (uppercase ? 'A' : 'a');
    //     }
    //     val /= base;
    // }
    // i32 len = i;
    // while(i>0) buffer[j++] = temp[--i];
    // return len;
    
    const i8* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    while(val > 0 && i < 64) {
        temp[i++] = digits[val % base];
        val /= base;
    }

    i32 len = i, j = 0;
    while(i >0) buffer[j++] = temp[--i];
    free(temp);
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

    len += itos(ipart, buffer + len, 10, 0);

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

i32 ftoes(f64 val, i8* buffer, i32 precision, i32 uppercase) {
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
    len+= itos((u64)abs_ex, buffer + len, 10, uppercase);
    return len;
}
