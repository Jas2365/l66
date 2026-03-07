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

void _start_c() {
   if(AllocConsole()){
    size_t sz = 1024;
ssize_t ssz = -512;

i32 kk = 23;
printf("kk : %u\n", kk);
printf("Size a : %zu\n", sz);      // prints: Size: 1024
printf("Signed: %zd\n", ssz);   // prints: Signed: -512
printf("Hex: %#zx\n", sz);
        
        pause();
    }

    exit(exit_success);
}