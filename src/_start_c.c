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

null _start_c() {

    s64 s = 1024;
    pd64 d = 50;
    m64 m = 0xFFFFFFFFFFFFFFFF;

    printf("Size: %zu, Diff: %td, Max: %ju\n", s, d, m);

    f64 num = 0.1;    
    u8* p = (u8*)&num;

    // printf("Little Endian (34 12) | big endian (12 34)\n");
    u64 num2  = 0;
    for(i32 i = 7; i >= 0; i--){
        printf("%02x ", *(p+i));
    }
    printf("\n");

    f64 num3 = 1.3725489;
  
    // printf("%x ", *p);
    // printf("%x ", *p+2);
    
    printf("\n");
    
    printf("%x %x %x\n", 23);

    printf("%zu\n", 0x7FFFFFFFFFFFFFFFULL);

    f64 kk = max_f32 + 13.8e10;
    printf("aaaa %e | %e %f \n", 23.3, 3.4, 111.25124542151);
    printf("%d\n",printf("0%d\n",printf("%dss\n",printf("aaaa %e | %f \n",11.11, 3.15 ))));
    printf("%d\n",sizeof(f64));
    printf("%d\n",23);
    printf("%i\n",23);
    printf("%f\n",23.0f);
    printf("%g\n",23.0f);                   // the g and e are not working 
    printf("%e\n",23.0f);
    printf("%c\n",236);
    printf("%c\n",2365);
    printf("%ls\n",L"one");
    printf("%p\n",&kk);
    printf("%%\n",&kk);
    i32 l = 0;
    printf("123 %n\n", &l);
    printf("%d\n", l);
    
    printf("%d %d %d\n", 23, 33, 45);
    printf("%s %c\n", "hello", 'w');
    printf("%qq %qnn\n", "hello", 'w');
    
    printf("%*d\n",10, 5);
    
    printf("%d \n\n", 23);
    printf("%.8d %-8d \n\n", 2365, 2365);
    printf("%-20f %-20f %n \n", 23.0, 23.0, &l);
    printf("l: %d\n", l);



    f32 ff  = 0.10;
    f64 fff = 0.10;

    printf("ff : %.2f\n", ff);

    u8* ptr = (u8*)&ff;
    for(i32 i = 7; i >= 0; i--)
    printf("%02x ", *(ptr+i));
    printf("\n");
    
    u8 bytes[8];
    memset(bytes, 0, 8);
    memcpy(bytes, &ff, 8);
    
    for(i32 i = 7; i >= 0; i--)
    printf("%02x ", bytes[i]);
    printf("\n");
    
    u64* mm = (u64*)&bytes;

    printf("x : %x  \n", *mm);
    printf("A : %a  \n",  ff);
    printf("A : %la \n", fff);
    printf("A : %a  \n", fff);
    
    pause();

    exit(exit_success);

}