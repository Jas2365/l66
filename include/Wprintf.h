#include "Wwin.h"
#include "Wmem.h"
#include "Wstring.h"

typedef __builtin_va_list va_list;
#define va_start(v, l)   __builtin_va_start(v,l)
#define va_arg(v, t)     __builtin_va_arg(v, t)
#define va_end(v)        __builtin_va_end(v)

#define stdout GetStdHandle(STD_OUTPUT_HANDLE)
#define stdin  GetStdHandle(STD_INPUT_HANDLE)

typedef struct fmt_spec {
    i32 flags;
    i32 width;
    i32 precision;
    i32 is_long_long; 
} fmt_spec_t, *fmt_spec_ptr_t;

enum fmt_sp: i32 {
    fmt_left  = 0x01,
    fmt_plus  = 0x02,
    fmt_space = 0x04,
    fmt_zero  = 0x08,
    fmt_alt   = 0x10,
};

#define flag_left  '-'
#define flag_plus  '+'
#define flag_space ' '
#define flag_zero  '0'
#define flag_alt   '#'

#define dyn_width  '*'
#define char_zero  '0'
#define char_nine  '9'
#define char_space ' '

#define char_format '%'
#define char_minus  '-'
#define char_plus   '+'
#define char_space  ' '
#define char_lx     'x'
#define char_ux     'X'
#define char_o      'o'


#define decimal_point  '.'
#define is_long        'l'

#define default_precision 6

static void flush_buffer(const i8* buffer, i32 len){
    if(len > 0){
        u32 written = 0;
        WriteFile(stdout, buffer, len, &written, 0);
    }
}

static void reset_fmt(fmt_spec_ptr_t spec) {
    spec->flags = 0;
    spec->width = 0;
    spec->precision = -1;
    spec->is_long_long = false;
}

static boolean is_flag(const i8* p) {
    return (
        *p == flag_left  ||
        *p == flag_plus  ||
        *p == flag_space ||
        *p == flag_zero  ||
        *p == flag_alt 
    );
}

static i32 check_flags(fmt_spec_ptr_t spec, i8* buff, i32 len){
    if     (spec->flags & fmt_plus)  buff[len++] = flag_plus;
    else if(spec->flags & fmt_space) buff[len++] = flag_space;
    return len;
}

//
// Parse Format Specifier: %[flags][width][.precision][length specifier]
//
static const i8* parse_fmt(const i8* p, fmt_spec_ptr_t spec, va_list args) {
    reset_fmt(spec);

    // flags
    while(*p && is_flag(p)) {
        if      (*p == flag_left  )  spec->flags |= fmt_left;
        else if (*p == flag_plus  )  spec->flags |= fmt_plus;
        else if (*p == flag_space )  spec->flags |= fmt_space;
        else if (*p == flag_zero  )  spec->flags |= fmt_zero;
        else if (*p == flag_alt   )  spec->flags |= fmt_alt;
        p++;
    }

    // width
    if(*p == dyn_width){
        spec->width = va_arg(args, i32);
        p++;
    } else {
        while(*p >= char_zero && *p <= char_nine){
            spec->width = spec->width * 10 + (*p - char_zero);
            p++;
        }
    }

    // Precision
    if(*p == decimal_point) {
        p++;
        spec->precision = 0;
        if(*p == dyn_width) {
            spec->precision = va_arg(args, i32);
            p++;
        } else {
            while(*p >= char_zero && *p <= char_nine) {
                spec->precision = spec->precision * 10 + (*p - char_zero);
                p++;
            }
        }
    }

    // length long long
    if(*p == is_long){
        p++;
        if(*p == is_long) {
            spec->is_long_long = 1;
            p++;
        }
    }

    return p;
}

static i32 apply_padding(i8* buffer, i32 len, const fmt_spec_ptr_t spec, i32 is_negative) {
    if(len >= spec->width) return len; // no padding

    i32 pad_count = spec->width - len;
    i32 total_len = spec->width;

    // left alignment
    if(spec->flags & fmt_left) {
        for(i32 i = len; i< total_len; i++) buffer[i] = char_space;
        return total_len;
    }

    // right alignment
    i8 pad_char = (spec->flags & fmt_zero) ? char_zero : char_space;

    // sign shifting
    i32 sign_offset = 0;
    if(is_negative || (spec->flags & fmt_plus) || (spec->flags & fmt_space)) {
        sign_offset = 1;
    }

    if(pad_char == char_zero && sign_offset) {
        // move sign to the fromt -123 becomes -0000123;
        for(i32 i = len -1; i>= sign_offset; i--) {
            buffer[i + pad_count] = buffer[i];
        }
        for(i32 i = sign_offset; i< sign_offset + pad_count; i++) {
            buffer[i] = char_zero;
        }
    } else {
        for(i32 i = len -1; i>=0; i--){
            buffer[i + pad_count] = buffer[i];
        }
        for(i32 i = 0; i< pad_count; i++) {
            buffer[i] = pad_char;
        }
    }
    return total_len;
}

#define buf_size 4096
#define tm_buf_size 1024
#define flush_buff_limit 1024

void printf(const i8* format, ...){
    va_list args;
    va_start(args, format);

    static i8  buffer[buf_size];
    static i8  tm_buf[tm_buf_size];
    static i32 b_idx = 0;

    for(const i8* p = format; *p != '\0'; p++) {
        if(b_idx >= flush_buff_limit) {
            flush_buffer(buffer, b_idx);
            b_idx = 0;
        }

        if(*p != char_format){
            buffer[b_idx++] = *p;
            continue;
        }
        p++; //skip %

        if(*p  == char_format){
            buffer[b_idx++] = char_format;
            continue;
        }

        // Parser 
        fmt_spec_t spec;
        p = parse_fmt(p, &spec, args);
        // p--;                                                      // ! could be a bug [ bug ]

        // format specifiers
        u64 val;
        f64 valf;
        i32 temp_len = 0;
        i32 is_negative = 0;
        i32 prec;
        switch (*p) {
        case 'd':
        case 'i': val = spec.is_long_long ? va_arg(args, i64) : va_arg(args, i32);
                  if((i64)val < 0) {
                      tm_buf[temp_len++] = char_minus;
                      is_negative = 1;
                      val = (i64) val == -val ? val : -val;
                      temp_len = 1 + itos((u64)val, tm_buf + 1 ,10, 0);
                  } else {
                    if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
                    else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
                    // temp_len = check_flags(&spec, tm_buf, temp_len);
                    temp_len += itos((u64)val, tm_buf + temp_len, 10, 0);
                  }
                  break;
        case 'f': valf = va_arg(args, f64);
                  if(valf < 0) tm_buf[temp_len++] = char_minus;
                  else if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
                  else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
                  prec = (spec.precision >= 0) ? spec.precision : default_precision; 
                  temp_len += ftos(valf, tm_buf + temp_len, prec);
                  break;
        case 'e':
        case 'E': valf = va_arg(args, f64);
                  if(valf < 0) tm_buf[temp_len++] = char_minus;
                  else if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
                  else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
                  prec = (spec.precision >= 0) ? spec.precision : default_precision;
                  temp_len += ftoes(valf, tm_buf + temp_len, prec, (*p == 'E'));
                  break;
        case 'g':
        case 'G': valf = va_arg(args, f64);
                  if(val < 0) tm_buf[temp_len++] = char_minus;
                  else if(spec.flags & fmt_plus) tm_buf[temp_len++] = char_plus;
                  else if(spec.flags & fmt_space) tm_buf[temp_len++] = char_space;
                  f64 absv = (valf < 0) ? -valf : valf;
                  prec = (spec.precision >= 0) ? spec.precision : default_precision;
                  if(absv != 0.0 && (absv >= 1000000.0 || absv < 0.0001)) {
                    temp_len += ftoes(valf, tm_buf + temp_len, prec -1 , (*p == 'G'));
                  } else {
                    temp_len += ftos(valf, tm_buf +temp_len, prec);
                  }
                  break;
        case 'u': val = spec.is_long_long ? va_arg(args, u64) : va_arg(args, u32);
                  temp_len += itos(val, tm_buf, 10, 0);
                  break;
        case 'x': 
        case 'X': val = spec.is_long_long ? va_arg(args, u64) : va_arg(args, u32);
                 if ((spec.flags & fmt_alt) && val != 0) {
                    tm_buf[temp_len++] = char_zero;
                    tm_buf[temp_len++] = (*p == char_ux) ? char_ux : char_lx;
                 }
                 else {
                    temp_len += itos(val, tm_buf , 16, (*p == 'X'));
                 }
                 break;
        case 'o': val = spec.is_long_long ? va_arg(args, u64) : va_arg(args, u32);
                  if((spec.flags & fmt_alt) && val != 0){
                    tm_buf[temp_len++] = char_zero;
                    tm_buf[temp_len++] = char_o;
                    temp_len += itos(val, tm_buf + temp_len, 8, 0);
                  } else {
                    tm_buf[temp_len++] = char_zero;
                    tm_buf[temp_len++] = char_o;
                    temp_len += itos(val, tm_buf +temp_len, 8, 0);
                  }
                break;
        case 'p': void* ptr = va_arg(args, void*);
                  tm_buf[temp_len++] = char_zero;
                  tm_buf[temp_len++] = char_lx;
                  temp_len += itos((u64)ptr, tm_buf + temp_len, 16, 0);
                break;
        case 's': const i8* s = va_arg(args, const i8*);
                  if(!s) s = "(null)";
                  i32 max_chars = (spec.precision>= 0) ? spec.precision : -1;
                  while(*s && (max_chars < 0 || temp_len < max_chars)) {
                    tm_buf[temp_len++] = *s++;
                  }
                break;
        case 'c': tm_buf[temp_len++] = (i8)va_arg(args, i32);
                break;
        case '%': tm_buf[temp_len++] = char_format;
                break;
        default:              
                tm_buf[temp_len++] = char_format;                
                tm_buf[temp_len++] = *p;                
            break;
        }

        // padding
        temp_len = apply_padding(tm_buf, temp_len, &spec, is_negative);

        // copy to main buffer
        for(i32 i = 0; i< temp_len; i++){
            if(b_idx >= flush_buff_limit) {
                flush_buffer(buffer, b_idx);
                b_idx = 0;
            }
            buffer[b_idx++] = tm_buf[i];
        }
    }

    flush_buffer(buffer, b_idx);
    va_end(args);
}