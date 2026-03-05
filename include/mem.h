#pragma once
#include "types.h"
#include "win.h"

void* malloc(size_t size) {
    HANDLE heap = GetProcessHeap();
    if(!heap) return NULL;
    return HeapAlloc(GetProcessHeap(), 0, size);
}

void free(void* ptr){
    if(ptr) HeapFree(GetProcessHeap(), 0, ptr);
}