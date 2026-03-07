# Implementation Progress Report

**Project:** LeanAPI (leap) - Minimalist C Standard Library For Windows x64  
**Date:** March 7, 2026  
**Status:** Beta (Core Features Complete)

---

## Overview

LeanAPI is a freestanding C standard library implementation for Windows x64 that eliminates dependency on the Microsoft Visual C Runtime (MSVCRT). The project provides essential functionality for bare-metal or size-constrained Windows applications by directly interfacing with Windows kernel APIs.

---

## Implementation Summary

### Implemented Features

#### 1. **Custom Printf Implementation**
- **Format Specifiers:**
  - Integer: `%d`, `%i`, `%u`
  - Floating-point: `%f`, `%e`, `%E`, `%g`, `%G`
  - Hex/Octal: `%x`, `%X`, `%o`
  - Pointer: `%p`
  - Character/String: `%c`, `%s`
  - Literal: `%%`
  
- **Flag Support:**
  - Left alignment: `-`
  - Sign display: `+`
  - Space for positive: (space)
  - Padding: `0`
  - Alternate form: `#`
  
- **Width & Precision:**
  - Fixed width specification (e.g., `%10d`)
  - Dynamic width via `*` (e.g., `%*d`)
  - Precision for floats and strings (e.g., `%.6f`)
  - Dynamic precision via `*` (e.g., `%.*f`)
  
- **Length Modifiers:**
  - Long: `l` (for `i32`/`u32` → `i64`/`u64`)
  - Long long: `ll` (for `i64`/`u64`)
  - Size type: `z` (for `size_t`/`ssize_t`) - **NEW**
  
- **Output:** Direct console I/O via Windows `WriteFile()` API

#### 2. **Memory Management**
- `malloc()` - Allocates memory using Windows process heap
- `free()` - Deallocates memory from process heap
- Direct `GetProcessHeap()` integration
- Efficient heap-based allocation without MSVCRT

#### 3. **Windows API Wrappers** (Wwin.h)
- Console allocation: `AllocConsole()`
- I/O handles: `GetStdHandle()`
- File operations: `WriteFile()`, `ReadFile()`
- Memory operations: `HeapAlloc()`, `HeapFree()`, `GetProcessHeap()`
- Process control: `ExitProcess()`

#### 4. **Type System** (100% Complete)
- **Unsigned integers:** `u8`, `u16`, `u32`, `u64`
- **Signed integers:** `i8`, `i16`, `i32`, `i64`
- **Floating-point:** `f32`, `f64`
- **Pointer types:** `size_t`, `ssize_t`, `uintptr_t`
- **Boolean:** `boolean` (true/false)
- **String utilities:** `const i8*` (null-terminated C strings)

#### 5. **Utility Functions**
- String operations (Wstring.h)
- Numeric conversions: `itos()`, `ftos()`, `ftoes()`
- Math utilities (Wmath.h)
- Memory utilities (Wmem.h)

#### 6. **Entry Point Customization**
- Assembly entry point (`_start_s.s`):
  - Stack alignment (16-byte boundary - Windows x64 requirement)
  - Shadow space allocation (32-byte)
  - Jump to C entry point
- C runtime entry point (`_start_c()`)
- Complete control without MSVCRT initialization

#### 7. **Compilation & Linking**
- Freestanding compilation (`-ffreestanding`)
- No standard library (`-nostdlib`)
- No built-in functions (`-fno-builtin`)
- Size optimization (`-Os`)
- Static linking with only `kernel32.dll`
- Symbol stripping for minimal binary size

---

## Implementation Limitations

### ❌ Not Implemented

#### 1. **Printf Format Specifiers**
- `%n` - Pointer to output character count (security risk)
- `%a`, `%A` - Hexadecimal floating-point notation
- `%i` base detection (currently treats as `%d`)

#### 2. **Printf Length Modifiers**
- `h` - Short integer (16-bit)
- `hh` - Char integer (8-bit)
- `j` - `intmax_t` / `uintmax_t`
- `t` - `ptrdiff_t`
- `L` - `long double`

#### 3. **I/O Operations**
- **File I/O:** No `fopen()`, `fclose()`, `fread()`, `fwrite()`
- **Buffered I/O:** Limited buffering control
- **Binary I/O:** No structured binary I/O functions
- **Stream operations:** No FILE structure implementation

#### 4. **Advanced Features**
- **Exception Handling:** No C++ exceptions or SEH (Structured Exception Handling)
- **Thread Support:** No thread creation, synchronization, or thread-local storage
- **Dynamic Loading:** No `LoadLibrary()` or `GetProcAddress()` wrappers
- **Registry Access:** No Windows registry API wrappers
- **Unicode:** No wide character support (`wchar_t`, `wcprintf()`)

#### 5. **Standard Library Functions**
- `qsort()`, `bsearch()` - Sorting and searching
- `rand()`, `srand()` - Pseudo-random number generation
- `setjmp()`, `longjmp()` - Exception/error handling jumps
- `signal()`, `atexit()` - Signal handling and exit callbacks
- `getenv()`, `putenv()` - Environment variable access
- `abort()` - Hard process termination

#### 6. **Floating-Point Edge Cases**
- `NaN` (Not a Number) handling incomplete
- `Inf` (Infinity) representation incomplete
- Subnormal number support limited
- Rounding mode selection not available

#### 7. **Printf Precision Semantics**
- Some edge cases with precision and flags not fully tested
- Floating-point precision may differ from standard library in extreme ranges
- String precision truncation tested but not exhaustively

---

## Platform Constraints

### System Requirements
- **OS:** Windows x64 (64-bit operating system)
- **Architecture:** x86-64 (AMD64)
- **Compiler:** GCC (MinGW-w64)
- **Assembler:** GNU Assembler (gas)

### Not Supported
- 32-bit Windows (x86)
- Non-Windows operating systems (Linux, macOS, etc.)
- ARM or other architectures
- MSVC compiler or Clang (untested)

---

## Performance Characteristics

### Strengths
- **Minimal Overhead:** No MSVCRT initialization
- **Direct System Calls:** Reduced indirection through win32 API
- **Fixed-Width Types:** Predictable memory layout
- **Size-Optimized:** `-Os` compilation flag keeps binary small

### Trade-offs
- **Single Buffer:** Printf uses static buffers (4KB main, 1KB temp)
- **No Caching:** Each printf triggers I/O operations
- **Stack Alignment:** Required 16-byte stack alignment adds setup code

---

## Testing Status

### Verified
- Basic printf functionality (all standard specifiers)
- Width and precision handling
- Flag combinations
- Memory allocation and deallocation
- Size type (%z) for size_t and ssize_t
- Console I/O on Windows
- Buffer flushing and duplicate output fix

### Not Verified
- Floating-point edge cases (subnormal, very large exponents)
- Thread safety (not designed for multithreading)
- Large buffer operations (>4KB)
- Unicode console output

---

## Code Quality

### Strengths
- Well-structured, modular design
- Clear separation of concerns (types, string, memory, I/O, math)
- Documented format specifier parsing
- Defensive programming (null pointer checks)

### Areas for Improvement
- More comprehensive error handling
- Extended test coverage for edge cases
- Performance benchmarking
- Security audit for buffer overflows

---

## Future Roadmap

### High Priority
1. Add `%h`, `%hh` length modifiers
2. Implement file I/O operations
3. Add unicode support (`wchar_t`, wide printf)
4. Implement `qsort()`, `bsearch()`

### Medium Priority
1. Add random number generation (`rand()`, `srand()`)
2. Implement environment variable access
3. Add registry API wrappers
4. Improve floating-point edge case handling

### Low Priority
1. Unicode normalization utilities
2. Advanced threading primitives
3. Performance optimization (AVX, SSE)
4. 32-bit Windows support

---

## Conclusion

LeanAPI provides a **functional, lightweight C standard library** for Windows x64 applications that need to avoid MSVCRT overhead. The implementation covers **essential printf functionality** with proper format specifier and modifier support, **complete memory management**, and **direct Windows API integration**.

While several advanced features remain unimplemented, the core functionality is robust and sufficient for most bare-metal and size-constrained Windows applications. The library successfully demonstrates **zero-dependency compilation** using only `kernel32.dll` while maintaining compatibility with standard C programming patterns.

## Implements Completed
**Memory Management**
- `malloc`: Wraps `HeapAlloc(GetProcessHeap(), 0, size)`
- `free`: Wraps `HeapFree` against the process heap

**I/O**
- `stdout`: Mapped to `GetStdHandle(STD_OUTPUT_HANDLE)`
- `stdin`: Mapped to `GetStdHandle(STD_INPUT_HANDLE)`
- `printf`
--   `specifiers`: `%i`, `%d`, `%f`, `%e`, `%E`, `%g`, `%G`, `%u`, `%x`, `%X`, `%o`, `%p`, `%c`, `%s`, `%%` and `default`
--   `modifiers`
--   `flags` : `left` `plus` `space` `zero` `alt`
--   `width` : can specify width, also `*` dynamic with specifier 
--   `precision` : can specify precison of both string and floating points
--   `length` : `l` (long), `ll` (long long), `z` (size_t/ssize_t)

**Data Types**
Fixed-width integer types ensure consistent behavior across platforms:
- `Unsigned`: `u8`, `u16`, `u32`, `u64`
- `Signed`: `i8`, `i16`, `i32`, `i64`
- `Pointers`: `size_t`, `ssize_t`, `uintptr_t`
- `Floating`: `f32`, `f64`
- `Boolean`: `boolean`
