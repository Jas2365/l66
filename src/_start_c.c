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
#include "Wdefs.h"
#include "Wprintf.h"
#include "Wmath.h"
// null inspect_f128_bits() {
//     f128 test_val = 1.0L;
//     u8 bytes[16];
//     Wmemset(bytes, 0, 16);
//     Wmemcpy(bytes, &test_val, 16);

//     printf("Raw Bytes of 1.0L: ");
//     for(i32 i = 0; i < 16; i++) {
//         // Using your hex logic to see the raw memory
//         printf("%02x ", bytes[i]);
//     }
//     printf("\n");
// }
null _start_c() {

    f32 num = 1.3725489f;    
    u8* p = (u8*)&num;

    // printf("Little Endian (34 12) | big endian (12 34)\n");
    i32 num2  = 0;
    for(i32 i = 0; i < 4; i++){
        printf("%02x ", *(p+i));
    }
    printf("\n");

    num2 = (*(p+3) << 8+8+8) | (*(p+2) << 8+8) | (*(p+1) << 8) |  (*(p));
    printf("%#x ", num2);

    printf("\n");

    f64 num3 = 1.3725489;


   
    // printf("%x ", *p);
    // printf("%x ", *p+2);

    printf("\n");

    // inspect_f128_bits();

    // printf("%zu", 0x7FFFFFFFFFFFFFFFULL);
    // printf("%La\n", 0.23L);

    // f64 kk = max_f32 + 13.8e10;
    // printf("aaaa %e | %e %Lf \n", 23.3, 3.4, 111.25124542151L);
    // printf("%d\n",printf("0%d\n",printf("%dss\n",printf("aaaa %e | %Lf \n",11.11, 3.15 ))));
    // printf("%d\n",sizeof(f128));
    // printf("%d\n",23);
    // printf("%i\n",23);
    // printf("%f\n",23.0f);
    // printf("%g\n",23.0f);
    // printf("%e\n",23.0f);
    // printf("%c\n",236);
    // printf("%c\n",2365);
    // printf("%ls\n",L"one");
    // printf("%p\n",&kk);
    // printf("%%\n",&kk);
    // i32 l = 0;
    // printf("123 %n\n", &l);
    // printf("%d\n", l);
    pause();

    exit(exit_success);

}