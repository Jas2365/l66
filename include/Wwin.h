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
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, size, &read, NULL);
}

void pause(){
    print_console("\nEnter any Key to continue...");
    read_console(1);
}