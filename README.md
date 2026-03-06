# TinyWinStd: Minimalist C Standard Library

An ultra-lightweight, freestanding implementation of the C Standard Library for **Windows x64**. This project bypasses the standard MSVCRT (Microsoft Visual C Runtime) and links directly against **kernel32.dll**, providing a minimal runtime for bare-metal or size-constrained Windows applications.

## Table of Contents
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Limitations](#limitations)
- [Implementation](#Implementation)

## Features

- **Zero Runtime Dependencies**: Compiled with `-nostdlib` and `-ffreestanding`, eliminating dependency on `msvcrt.dll` or any standard C runtime libraries
- **Direct Win32 Integration**: Wraps Windows kernel APIs (`HeapAlloc`, `WriteFile`, `GetStdHandle`) for full control over system calls
- **Custom Printf**: Lightweight `printf` implementation
- **Memory Management**: `malloc` and `free` implemented using Windows process heap API (`GetProcessHeap`)
- **Complete Entry Point Control**: Custom assembly (`_start_s`) and C entry point (`_start_c`) instead of standard runtime initialization
- **Fixed-Width Types**: Portable type definitions (`u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`, `size_t`, `ssize_t`, `uintptr_t`)
- **Minimal Binary Size**: Optimized with `-Os` and stripped of all non-essential symbols

## Prerequisites

- **GCC** (MinGW-w64 for Windows)
- **GNU Assembler** (as)
- **Make** (for Windows - included with MinGW)
- **Windows x64 OS**

## Quick Start

### Build
```bash
make all
```

### Build and Run
```bash
make run
```

### Clean Build Artifacts
```bash
make clean
```

The compiled executable is generated at `out/main.exe`.

## Project Structure

```
.
├── Makefile              # Build configuration
├── README.md             # This file
├── include/              # Header files
│   ├── Wdefs.h           # Core definitions
│   ├── Wmath.h           # Math utilities
│   ├── Wmem.h            # Memory management
│   ├── Wprintf.h         # Printf implementation
│   ├── Wstring.h         # value to string implementation
│   ├── Wtypes.h          # Type definitions
│   └── wwin.h            # Windows API wrappers
└── src/                 # Source files
    ├── _start_s.s       # Assembly entry point
    └── _start_c.c       # C runtime initialization
```

## Usage

Include the standard headers in your C code:

```c
#include "include/Wprintf.h"
#include "include/Wmem.h"
#include "include/Wtypes.h"

void _start_c(void) {
    printf("Hello from TinyWinStd!\n");
    
    u8 *buffer = malloc(256);
    if (buffer) {
        printf("Allocated 256 bytes\n");
        free(buffer);
    }
}
```

## Technical Details

### Entry Point

The program control flow begins in assembly (`_start_s`):
1. Aligns the stack to 16-byte boundary (Windows x64 requirement)
2. Allocates 32-byte shadow space for Windows x64 calling convention
3. Transfers control to the C entry point (`_start_c`)

### Memory and I/O

- **stdout**: Mapped to `GetStdHandle(STD_OUTPUT_HANDLE)`
- **stdin**: Mapped to `GetStdHandle(STD_INPUT_HANDLE)`
- **malloc**: Wraps `HeapAlloc(GetProcessHeap(), 0, size)`
- **free**: Wraps `HeapFree` against the process heap

### Type System

Fixed-width integer types ensure consistent behavior across platforms:
- **Unsigned**: `u8`, `u16`, `u32`, `u64`
- **Signed**: `i8`, `i16`, `i32`, `i64`
- **Pointers**: `size_t`, `ssize_t`, `uintptr_t`
- **Floating**: `f32`, `f64`
- **Boolean**: `boolean`

### Compiler Flags

- `-ffreestanding`: No standard library assumptions
- `-nostdlib`: Don't link standard libraries
- `-fno-builtin`: Disable compiler built-in functions
- `-fno-pie`: Position-independent code disabled
- `-Os`: Optimize for size

## Limitations

- **Windows x64 Only**: Platform-specific assembly and Win32 API calls limit portability
- **Console I/O Only**: Input/output restricted to standard console handles; file I/O not currently implemented
- **No Exception Handling**: C++ features and structured exception handling not supported
- **Single-Threaded**: No thread synchronization primitives

## Implementation
 **Printf**: 
-   *specifiers* 
-    `%i`, `%d`, `%f`, `%e`, `%E`, `%g`, `%G`, `%u`, `%x`, `%X`, `%o`, `%p`, `%c`, `%s`, `%%` and `default`
-   *modifiers*
-   `flags` : `left` `plus` `space` `zero` `alt`
-   `width` : can specify width, also `*` dynamic with specifier 
-   `precision` : can specify precison of both string and floating points


*Date*: `05-March-2026`
