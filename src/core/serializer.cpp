#include "serializer.h"

void Serializer::pack(Buffer &buffer, size_t bytesUsed, u8 value) {
  assert(bytesUsed + 1 <= buffer.getSize());
  uchar *data = buffer.data() + bytesUsed;
  *data = static_cast<uchar>(value);
}

void Serializer::pack(Buffer &buffer, size_t bytesUsed, u16 value) {
  assert(bytesUsed + 2 <= buffer.getSize());
  uchar *data = buffer.data() + bytesUsed;
  *data++ = value >> 8;
  *data = value;
}

void Serializer::pack(Buffer &buffer, size_t bytesUsed, u32 value) {
  assert(bytesUsed + 4 <= buffer.getSize());
  uchar *data = buffer.data() + bytesUsed;
  *data++ = value >> 24;
  *data++ = value >> 16;
  *data++ = value >> 8;
  *data = value;
}

void Serializer::pack(Buffer &buffer, size_t bytesUsed, u64 value) {
  assert(bytesUsed + 8 <= buffer.getSize());
  uchar *data = buffer.data() + bytesUsed;
  *data++ = value >> 56;
  *data++ = value >> 48;
  *data++ = value >> 40;
  *data++ = value >> 32;
  *data++ = value >> 24;
  *data++ = value >> 16;
  *data++ = value >> 8;
  *data = value;
}

u64 Serializer::pack754(long double value, unsigned bits, unsigned expbits) {
  long double vnorm;
  int shift = 0;
  u64 sign, exp, mantissa;
  unsigned manbits = bits - expbits - 1;

  if (value == 0.0)
    return 0;

  // sign
  if (value < 0.0) {
    sign = 1;
    vnorm = -value;
  } else {
    sign = 0;
    vnorm = value;
  }

  // normalize value and calcuate number of shits required to normalize;
  while (vnorm >= 2.0) {
    vnorm /= 2.0;
    ++shift;
  }
  while (vnorm < 1.0) {
    vnorm *= 2.0f;
    --shift;
  }
  vnorm = vnorm - 1.0f;

  // calcuate mantissa, +0.5f to round off large vnorm values
  mantissa = vnorm * ((1LL << manbits) + 0.5f);
  exp = shift + ((1LL << (expbits - 1)) - 1); // adding bias (2^(n-1)-1)
  // combine all
  return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | mantissa;
}

long double Serializer::unPack754(u64 value, unsigned bits, unsigned expbits) {
  long double result;
  long long shift;
  unsigned bias;
  unsigned manbits = bits - expbits - 1;

  if (value == 0)
    return 0.0;
  result = (value & ((1LL << manbits) - 1)); // extract mantissa
  result /= (1LL << manbits);                // convert to floating point
  result += 1.0;                             // back to normal form(normalize)

  // apply exponent
  bias = (1 << (expbits - 1)) - 1; // 2^(n-1)-1
  shift = ((value >> manbits) & ((1LL << expbits) - 1)) - bias;
  while (shift > 0) {
    result *= 2.0;
    --shift;
  }
  while (shift < 0) {
    result /= 2.0f;
    ++shift;
  }
  // add sign
  result *= (value >> (bits - 1)) & 1 ? -1.0 : 1.0;
  return result;
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        char &value) {
  auto data = *(buffer.data() + bytesUnPacked);
  if (data <= 0x7fu)
    // positive number (doesn't use sign bit)
    value = data;
  else
    // negative number
    // remove offset added by int -> uint cast
    value = -1 - (uchar)(0xffu - data);
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked, s8 &value) {
  auto data = *(buffer.data() + bytesUnPacked);
  if (data <= 0x7fu)
    value = data;
  else
    value = -1 - (u8)(0xffu - data);
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        s16 &value) {
  auto buf = buffer.data() + bytesUnPacked;
  u16 data = (u16)buf[0] << 8 | buf[1];
  if (data <= 0x7fffu)
    value = data;
  else
    value = -1 - (u16)(0xffffu - data);
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        s32 &value) {
  auto buf = buffer.data() + bytesUnPacked;
  u32 data =
      ((u32)buf[0] << 24) | ((u32)buf[1] << 16) | ((u32)buf[2] << 8) | buf[3];
  if (data <= 0x7fffffffu)
    value = data;
  else
    value = -1 - (u32)(0xffffffffu - data);
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        s64 &value) {
  auto buf = buffer.data() + bytesUnPacked;
  u64 data = ((u64)buf[0] << 56) | ((u64)buf[1] << 48) | ((u64)buf[2] << 40) |
             ((u64)buf[3] << 32) | ((u64)buf[4] << 24) | ((u64)buf[5] << 16) |
             ((u64)buf[6] << 8) | buf[7];
  if (data <= 0x7fffffffffffffffu)
    value = data;
  else
    value = -1 - (u64)(0xffffffffffffffffu - data);
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked, u8 &value) {
  value = static_cast<u8>(*(buffer.data() + bytesUnPacked));
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        u16 &value) {
  auto buf = buffer.data() + bytesUnPacked;
  value = (u16)buf[0] << 8 | buf[1];
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        u32 &value) {
  auto buf = buffer.data() + bytesUnPacked;
  value =
      ((u64)buf[0] << 24) | ((u64)buf[1] << 16) | ((u64)buf[2] << 8) | buf[3];
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        u64 &value) {
  auto buf = buffer.data() + bytesUnPacked;
  value = ((u64)buf[0] << 56) | ((u64)buf[1] << 48) | ((u64)buf[2] << 40) |
          ((u64)buf[3] << 32) | ((u64)buf[4] << 24) | ((u64)buf[5] << 16) |
          ((u64)buf[6] << 8) | buf[7];
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        float &value) {
  u32 bin;
  unPack(buffer, bytesUnPacked, bin);
  value = unPack754(bin, FLOAT_TOTAL_BITS, FLOAT_EXPONENT_BITS);
}

void Serializer::unPack(const Buffer &buffer, size_t bytesUnPacked,
                        double &value) {
  u64 bin;
  unPack(buffer, bytesUnPacked, bin);
  value = unPack754(bin, DOUBLE_TOTAL_BITS, DOUBLE_EXPONENT_BITS);
}
