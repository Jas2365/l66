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

#define upper_case_digits "0123456789ABCDEF"
#define lower_case_digits "0123456789abcdef"

#define itos_temp_size (65)

#define fractional_mantissa (9223372036854775807ULL)
#define def_hex_precision   (15)
#define exp_bias            (0x3fff)
#define exp_bias_neg        (-0x3ffe)
#define exp_limit           (0x7fff)
#define mantisa_mask        (0x7FFFFFFFFFFFFFFFULL)

i32 check_nan(i8* buffer, i32 len);
i32 check_max(i8* buffer, i32 len);
i32 check_min(i8* buffer, i32 len);

i32 itos(u64 val, i8* buffer, i32 base, boolean uppercase);

i32 f32tos(f64 val, i8* buffer, i32 precision);
i32 f64tos(f64 val, i8* buffer, i32 precision);


i32 f32toes(f64 val, i8* buffer, i32 precision, boolean uppercase);
i32 f64toes(f64 val, i8* buffer, i32 precision, boolean uppercase);


i32 utf16_to_utf8(const w16* wstr, i32 wlen, i8* buf, i32 max_buf_len);

// i32 f128tohex(f128 val, i8* buffer, i32 precision, boolean uppdercase);
// i32 f128toes(f128 val, i8* buffer, i32 precision, boolean uppercase);
// i32 f128tos(f128 val, i8* buffer, i32 precision);