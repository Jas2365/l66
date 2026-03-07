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
