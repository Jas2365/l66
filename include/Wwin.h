#pragma once
#include "Wtypes.h"
#include "Wdefs.h"

__declspec(dllimport) void   __stdcall ExitProcess(u32 uExitCode);
__declspec(dllimport) HANDLE __stdcall GetProcessHeap();
__declspec(dllimport) void*  __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) BOOL   __stdcall HeapFree (HANDLE hHeap, DWORD dwFlags, void* lpMem);
__declspec(dllimport) BOOL   __stdcall AllocConsole();
__declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
__declspec(dllimport) BOOL   __stdcall WriteFile(HANDLE hFile, const void* lpBuffer, DWORD nNumberOfBytesToWrite, DWORD* lpNumberOfBytesWritten, void* lpOverlapped);
__declspec(dllimport) BOOL   __stdcall ReadFile(HANDLE hFile, const void* lpbuffer, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead, void* lpOverlapped);

void exit(u32 exit_code){
    ExitProcess(exit_code);
}

void print_console(const char* str) {
    DWORD len = 0;
    while(str[len]) len++;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(hOut != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hOut, str,len,&written, NULL);
    }
}

void read_console(u32 size) {
    char buffer[size];
    DWORD read;
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &read, NULL);
}

void pause(){
    print_console("Enter any Key to continue...");
    read_console(1);
}