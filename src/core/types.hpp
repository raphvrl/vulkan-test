#pragma once

#include <cstdint>
#include <cfloat>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using usize = size_t;

#define I8_MIN  INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN

#define U8_MIN  0
#define U16_MIN 0
#define U32_MIN 0
#define U64_MIN 0

#define F32_MIN FLT_MIN
#define F64_MIN DBL_MIN

#define USIZE_MIN 0

#define I8_MAX  INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define F32_MAX FLT_MAX
#define F64_MAX DBL_MAX

#define USIZE_MAX SIZE_MAX

#define UNUSED(x) (void)(x)
