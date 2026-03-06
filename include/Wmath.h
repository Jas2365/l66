#pragma once
#include "Wtypes.h"

f64 lognf(f64 x) {
    if( x <= 0) return 0.0; // natural ln(-x) should be NaN or -Inf

    f64 res = 0.0;
    f64 y = ( x - 1.0)/ (x + 1.0);
    f64 y2 = y*y;
    f64 numerator = y;

    for(int i = 0; i< 50; i++){
        res += numerator / (2 * i + 1);
        numerator *= y2;
    }
    return 2.0 * res;
}

f64 expf(f64 x) {
    f64 res = 1.0;
    f64 term = 1.0;

    for(int i =1; i < 30; i++){
        term *= x/i;
        res += term;
    }
    return res;
}

f64 powi(f64 bx, i32 ey) {
    f64 res = 1.0;
    i64 p = (ey < 0) ? -(i64)ey : (i64)ey;

    while(p > 0) {
        if(p & 1) res *= bx;
        bx *= bx;
        p >>= 1;
    }
    return (ey < 0) ? 1.0 / res : res;
}


f64 pow(f64 bx, f64 ey) {

    if(ey == 0) return 1.0;
    if(bx == 0) return 0.0;
    if(bx <  0) {
        // real pow of negative numbers is comples (NaN)
        // unless exp is an integer
        if(ey == (i32)ey){
            f64 res = pow(-bx, ey);
            return ((i32)ey & 1 == 0) ? res : -res;
        }
    }

    if(ey == (i32)ey){
        return powi(bx, (i32)ey);
    }
    
    return expf((ey * lognf(bx)));
}

f64 pow10i(i32 e){
    f64 res = 1.0;
    if(e > 0) while(e--) res *=10.0;
    else while(e++) res /= 10.0;
    return res;
}


// static i32 f64_to_str(f64 val, i8* buf, i32 precision) {
//     i32 len = 0;
//     if (val < 0) { buf[len++] = '-'; val = -val; }

//     val += 0.5 * power10(-precision);

//     u64 ipart = (u64)val;
//     f64 fpart = val - (f64)ipart;
//     len += itos(ipart, buf + len, 10, 0);

//     if (precision > 0) {
//         buf[len++] = '.';
//         while (precision--) {
//             fpart *= 10.0;
//             i32 digit = (i32)fpart;
//             buf[len++] = digit + '0';
//             fpart -= digit;
//         }
//     }
//     return len;
// }

// static i32 f64_to_sci(f64 val, i8* buf, i32 precision, i32 uppercase) {
//     i32 len = 0;
//     if (val < 0) { buf[len++] = '-'; val = -val; }

//     i32 exponent = 0;
//     if (val != 0.0) {
//         while (val >= 10.0) { val /= 10.0; exponent++; }
//         while (val < 1.0) { val *= 10.0; exponent--; }
//     }

//     len += f64_to_str(val, buf + len, precision);
//     buf[len++] = uppercase ? 'E' : 'e';
//     buf[len++] = (exponent >= 0) ? '+' : '-';
    
//     u64 abs_exp = (exponent < 0) ? -exponent : exponent;
//     if (abs_exp < 10) buf[len++] = '0';
//     len += itos(abs_exp, buf + len, 10, 0);
    
//     return len;
// }


//  case 'f': fval = va_arg(args, f64);
//                   b_idx += f64_to_str(fval, buffer + b_idx, 6);
//                   break;
//         case 'e': fval = va_arg(args, f64);
//                   b_idx += f64_to_sci(fval, buffer + b_idx, 6, 0);
//                   break;
//         case 'E': fval = va_arg(args, f64);
//                   b_idx += f64_to_sci(fval, buffer + b_idx, 6, 1);
//                   break;
//         case 'g':
//         case 'G': fval = va_arg(args, f64);
//                   if (fval != 0.0 && (fval >= 1000000.0 || fval < 0.0001)) {
//                       b_idx += f64_to_sci(fval, buffer + b_idx, 4, (*p == 'G'));
//                   } else {
//                       b_idx += f64_to_str(fval, buffer + b_idx, 4);
//                   }
//                   break;