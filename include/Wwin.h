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

// ----- win types -----
typedef null *HANDLE;
typedef null *HWND;
typedef null *HINSTANCE;
typedef u32   DWORD;
typedef i32   BOOL;

// ----- win declaration -----
WINBASEAPI null   WINAPI ExitProcess(u32 uExitCode);

WINBASEAPI HANDLE WINAPI GetProcessHeap();
WINBASEAPI null*  WINAPI HeapAlloc(HANDLE hHeap, DWORD dwFlags, s64 dwBytes);
WINBASEAPI BOOL   WINAPI HeapFree (HANDLE hHeap, DWORD dwFlags, null* lpMem);

WINBASEAPI BOOL   WINAPI AllocConsole();

WINBASEAPI HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
WINBASEAPI BOOL   WINAPI WriteFile(HANDLE hFile, const null* lpBuffer, DWORD nNumberOfBytesToWrite, DWORD* lpNumberOfBytesWritten, null* lpOverlapped);
WINBASEAPI BOOL   WINAPI ReadFile (HANDLE hFile, const null* lpbuffer, DWORD nNumberOfBytesToRead,  DWORD* lpNumberOfBytesRead,    null* lpOverlapped);


// ----- my implementation -----
null exit(u32 exit_code);
null print_console(const char* str);
null read_console(u32 size);
null pause();