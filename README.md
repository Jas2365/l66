# LeanAPI (leap): Minimalist C Standard Library For Windows x64

An ultra-lightweight, freestanding implementation of the C Standard Library for **Windows x64**. This project bypasses the standard MSVCRT (Microsoft Visual C Runtime) and links directly against **kernel32.dll**, providing a minimal runtime for bare-metal or size-constrained Windows applications.

## Table of Contents
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Limitations](#limitations)
- [Documentation](#documentation)
- [License](#license)

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
    
void _start_c() {

   if(AllocConsole()){
        printf("Hello from TinyWinStd!\n");
        pause();

        exit(exit_success);
    }

    exit(exit_failure);
}
```

## Technical Details

### Entry Point

The program control flow begins in assembly (`_start_s`):
1. Aligns the stack to 16-byte boundary (Windows x64 requirement)
2. Allocates 32-byte shadow space for Windows x64 calling convention
3. Transfers control to the C entry point (`_start_c`)

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

## Documentation

For detailed information on implementation progress, supported features, and limitations, see **[PROGRESS.md](PROGRESS.md)**.

This includes:
- Complete list of implemented features
- Known limitations and unsupported functionality
- Platform constraints and system requirements
- Testing status and code quality notes
- Future roadmap and planned enhancements

## License

This project is licensed under the **Apache License 2.0**. See the [LICENSE](LICENSE) file for full details.

**Key Points:**
- Free to use for personal and commercial projects
- You may modify and distribute the software
- You must include a copy of the license
- Changes must be documented
- Software is provided "as-is" without warranty

*Date*: `07-March-2026`
