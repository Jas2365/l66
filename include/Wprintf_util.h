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

typedef struct fmt_spec {
    i32 flags;
    i32 width;
    i32 precision;
    i32 len_modifiers;
} fmt_spec_t, *fmt_spec_ptr_t;

null flush_buffer(const i8* buffer, i32 len);


boolean is_flag(const i8* p);
boolean is_modifer(const i8* p);


//
// Parse Format Specifier: %[flags][width][.precision][length specifier]
//
const i8* parse_fmt(const i8* p, fmt_spec_ptr_t spec, va_list args);
null reset_fmt(fmt_spec_ptr_t spec);

u64 u64_get_val(va_list *args, i32 len_mod);
f64 f64_get_val(va_list *args, i32 len_mod);

i32 apply_padding(i8* buffer, i32 len, const fmt_spec_ptr_t spec, i32 is_negative);

i32 f_util(i8* buf, fmt_spec_ptr_t spec);
