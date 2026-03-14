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

null exit(u32 exit_code){
    ExitProcess(exit_code);
}

null print_console(const char* str) {
    DWORD len = 0;
    while(str[len]) len++;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(hOut != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hOut, str,len,&written, VOID);
    }
}

null read_console(u32 size) {
    char buffer[size];
    DWORD read;
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, size, &read, VOID);
}

null pause(){
    print_console("Enter any Key to continue...");
    read_console(1);
}