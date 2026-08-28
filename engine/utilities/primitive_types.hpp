#pragma once

#include <cstdint>

// Signed integers
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

// Unsigned integers
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

// Floating numbers
using f32 = float;
using f64 = double;

// Non numeric values
using c8 = char; // Dangerous because C++ does not guarantee that 1 byte is 8 bits.