#include "win.h"
#include "mem.h"

typedef __builtin_va_list va_list;
#define va_start(v, l)   __builtin_va_start(v,l)
#define va_arg(v, t)     __builtin_va_arg(v, t)
#define va_end(v)        __builtin_va_end(v)

#define stdout GetStdHandle(STD_OUTPUT_HANDLE)
#define stdin  GetStdHandle(STD_INPUT_HANDLE)


static void flush_buffer(const i8* buffer, i32 len){
    if(len > 0){
        u32 written = 0;
        WriteFile(stdout, buffer, len, &written, 0);
    }
}

static i32 itos(u64 value, i8* buffer, i32 base, i32 uppercase) {
    i8* temp = (i8*)malloc(sizeof(i8) *65);
    i32 i = 0;

    const i8* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if(value == 0) {
        buffer[0] = '0';
        return 1;
    }

    while(value > 0 && i < 64) {
        temp[i++] = digits[value % base];
        value /= base;
    }

    i32 len = i;
    for(i32 j = 0; j< len; j++){
        buffer[j] = temp[len -1 -j];
    }
    free(temp);
    return len;
}

void printf(const i8* format, ...){
    va_list args;
    va_start(args, format);

    i8* buffer = (i8*)malloc(sizeof(i8) * 1024);
    int b_idx = 0;

    for(const i8* p = format; *p != '\0'; p++) {
        if(b_idx >= 1000) {
            flush_buffer(buffer, b_idx);
            b_idx = 0;
        }

        if(*p != '%'){
            buffer[b_idx++] = *p;
            continue;
        }
        p++; //skip %

        i32 is_long_long = 0;
        if(*p  == 'l'){
            p++;
            if(*p == 'l'){
                is_long_long = 1;
                p++;
            }
        }

        // format specifiers
        u64 val;
        switch (*p)
        {
        case 'd':
        case 'i': val = is_long_long ? va_arg(args, i64) : va_arg(args, i32);
                  if(val < 0) {
                      buffer[b_idx++] = '-';
                      val = -val;
                  }
                  b_idx += itos((u64)val, buffer + b_idx, 10, 0);
                  break;
        case 'f': f64 val = va_arg(args, f64);
                  // negative
                  if(val < 0) {
                    if(b_idx < 1023)
                       buffer[b_idx++] = '-';
                    val = -val;
                  }
                  // integer
                  u64 integer_part = (u64)val;
                  b_idx += itos(integer_part, buffer + b_idx, 10,0);

                  // decimal point
                  if( b_idx <1023) buffer[b_idx++] = '.';

                  // fractrion
                  f64 fractional_part = val - (f64)integer_part;
                  u64 fraction_as_int =(u64)(fractional_part * 1000000.0);

                  u64 temp_frac = fraction_as_int;
                  i32 precision = 6;
                  
                  i8 frac_buff[10];
                  i32 frac_len = itos(fraction_as_int, frac_buff, 10, 0);

                  for(i32 z = 0; z <(precision - frac_len); z++) {
                    if( b_idx < 1023) buffer[b_idx++] = '0';
                  }

                  for(i32 f = 0; f < frac_len; f++){
                    if(b_idx < 1023) buffer[b_idx++] = frac_buff[f]; 
                  }

                  break;
        case 'u': val = is_long_long ? va_arg(args, u64) : va_arg(args, u32);
                  b_idx += itos(val, buffer + b_idx, 10, 0);
                  break;
        case 'x': 
        case 'X': val = is_long_long ? va_arg(args, u64) : va_arg(args, u32);
                  b_idx += itos(val, buffer + b_idx, 16, (*p == 'X'));
                  break;
        case 'o': val = is_long_long ? va_arg(args, u64) : va_arg(args, u32);
                  b_idx += itos(val, buffer + b_idx, 8, 0);
                  break;
        case 'p': void* ptr = va_arg(args, void*);
                  buffer[b_idx++] = '0';
                  buffer[b_idx++] = 'x';
                  b_idx  += itos((u64)ptr, buffer + b_idx, 16, 0);
                  break;
        case 's': const i8* s = va_arg(args, const i8*);
                  if(!s) s = "(null)";
                  while(*s) {
                      if (b_idx > 1023) { 
                        flush_buffer(buffer, b_idx); 
                        b_idx = 0; 
                    }
                      buffer[b_idx++] =*s++;
                }
                break;
        case 'c': buffer[b_idx++] = (i8)va_arg(args, i32);
                break;
        case '%': buffer[b_idx++] = '%';
                break;
        default:
                buffer[b_idx++] = '%';                
                buffer[b_idx++] = *p;                
            break;
        val = 0;
        }


        
    }

    flush_buffer(buffer, b_idx);
    va_end(args);
    free(buffer);
}