#pragma once
#include "Wtypes.h"
#include "Wwin.h"

void* malloc(size_t size) {
    HANDLE heap = GetProcessHeap();
    if(!heap) return NULL;
    return HeapAlloc(GetProcessHeap(), 0, size);
}

void free(void* ptr){
    if(ptr) HeapFree(GetProcessHeap(), 0, ptr);
}