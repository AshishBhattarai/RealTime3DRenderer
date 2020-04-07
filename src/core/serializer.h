#pragma once

#include "buffer.h"
#include "common.h"
#include <iostream>
#include <utility>

class Serializer : NonCopyable {
public:
  static Serializer &getInstance() {
    static Serializer instance;
    return instance;
  }

  /**
   * Pack methos pack data into a provided buffer.
   * The provided buffer must be pre-allocated.
   *
   * bytesUsed should be zero initially.
   */
  void pack(Buffer &buffer, size_t bytesUsed, u8 value);
  void pack(Buffer &buffer, size_t bytesUsed, u16 value);
  void pack(Buffer &buffer, size_t bytesUsed, u32 value);
  void pack(Buffer &buffer, size_t bytesUsed, u64 value);

  template <typename T, typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, const T &value, Args &&... args) {
    pack(buffer, bytesUsed, value);
    pack(buffer, bytesUsed + sizeof(T), std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, s8 value, Args &&... args) {
    pack(buffer, bytesUsed, static_cast<u8>(value),
         std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, s16 value, Args &&... args) {
    pack(buffer, bytesUsed, static_cast<u16>(value),
         std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, s32 value, Args &&... args) {
    pack(buffer, bytesUsed, static_cast<u32>(value),
         std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, s64 value, Args &&... args) {
    pack(buffer, bytesUsed, static_cast<u64>(value),
         std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, char value, Args &&... args) {
    pack(buffer, bytesUsed, static_cast<u8>(value),
         std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, float value, Args &&... args) {
    u32 bin = pack754(value, FLOAT_TOTAL_BITS, FLOAT_EXPONENT_BITS);
    pack(buffer, bytesUsed, bin, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void pack(Buffer &buffer, size_t bytesUsed, double value, Args &&... args) {
    u64 bin = pack754(value, DOUBLE_TOTAL_BITS, DOUBLE_EXPONENT_BITS);
    pack(buffer, bytesUsed, bin, std::forward<Args>(args)...);
  }

  /**
   * unpack methods unpack given buffer based on provided template types
   *
   * bytesUnPacked should be zero initially
   */
  void unPack(const Buffer &buffer, size_t bytesUnPacked, s8 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, s16 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, s32 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, s64 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, u8 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, u16 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, u32 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, u64 &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, float &value);
  void unPack(const Buffer &buffer, size_t bytesUnPacked, double &value);

  template <typename T, typename... Args>
  void unPack(const Buffer &buffer, const size_t &bytesUnPacked, T &value,
              Args &... args) {
    unPack(buffer, bytesUnPacked, value);
    unPack(buffer, bytesUnPacked + sizeof(T), args...);
  }

private:
  static constexpr short FLOAT_TOTAL_BITS = 32;
  static constexpr short FLOAT_EXPONENT_BITS = 8;
  static constexpr short DOUBLE_TOTAL_BITS = 64;
  static constexpr short DOUBLE_EXPONENT_BITS = 11;

  Serializer() = default;
  ~Serializer() = default;

  /**
   * floating point conversion IEEE 754
   * float - 32-bit (8-bit exp + 23-bit mantissa)
   * double - 64-bit (11-bit exp + 52-bit mantissa)
   */
  u64 pack754(long double value, unsigned bits, unsigned expbits);
  long double unPack754(u64 value, unsigned bits, unsigned expbits);
};
