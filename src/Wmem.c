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

#include "Wmem.h"

void* malloc(s64 size) {
    HANDLE heap = GetProcessHeap();
    if(!heap) return NULL;
    return HeapAlloc(GetProcessHeap(), NULL, size);
}

void free(void* ptr){
    if(ptr) HeapFree(GetProcessHeap(), NULL, ptr);
}

void Wmemcpy(void* dest, const void* src, i32 count) {
    i8* d = (i8*)dest;
    const i8* s = (const i8*)src;
    while(count--){
        *d++ = *s++;
    }
}

void Wmemset(void* dest, i8 val, i32 count) {
    i8* d = (i8*)dest;
    while(count--){
        *d++ = val;
    }
}