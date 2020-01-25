#pragma once

#include <cstdint>

typedef unsigned char uchar;
typedef unsigned int uint;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uintptr_t uptr;

// will require for custom allocato
static_assert(sizeof(uint) == sizeof(u32), "Check conversion.");

// NoCopyable class
class NonCopyable {
protected:
  NonCopyable() = default;
  ~NonCopyable() = default;

  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
};